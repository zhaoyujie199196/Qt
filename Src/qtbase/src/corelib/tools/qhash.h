//
// Created by Yujie Zhao on 2023/8/4.
//

#ifndef QHASH_H
#define QHASH_H

#include "qhashfunctions.h"
#include "qrefcount.h"
#include "qiterator.h"
#include <QtCore/qmath.h>
#include <QtCore/qlist.h>
#include <iostream>

/*
 * QHash 不允许key重复的元素
 * QMultiHash允许key重复
 * Node为存放数据的节点，QHash为Node，QMultiHash为NodeChain
 * 插入元素，对Key做hash处理，hash碰撞往后面插入
 * 删除元素时，需要因为碰撞插入在后面的值前移填补空洞
 * */

class tst_QHash;
QT_BEGIN_NAMESPACE

template <class T>
class QSet;
template <class T, class Type>
class QMultiHash;

struct QHashDummyValue
{
    bool operator==(const QHashDummyValue &) const noexcept { return true; }
};

namespace QHashPrivate {
    //使用2的n次方的增长策略
    namespace GrowthPolicy {  //增长策略
        inline constexpr size_t maxNumBuckets() noexcept {
            //0x0111111;
            return size_t(1) << (8 * sizeof(size_t) - 1);
        }

        inline constexpr size_t bucketsForCapacity(size_t requestedCapacity) noexcept {
            if (requestedCapacity < 8) {  //小于8个，安排16个桶
                return 16;
            }
            if (requestedCapacity >= maxNumBuckets()) {
                return maxNumBuckets();
            }
            //容纳两倍的2的n次方的桶个数
            return qNextPowerOfTwo(QIntegerForSize<sizeof(size_t)>::Unsigned(2 * requestedCapacity - 1));
        }

        //hash值对应到的桶的序号
        inline constexpr size_t bucketForHash(size_t nBuckets, size_t hash) noexcept
        {
            return hash & (nBuckets - 1);
        }
    }
}

namespace QHashPrivate {

    //是否重载了QHash方法
    template <typename T, typename = void>
    constexpr inline bool HasQHashOverload = false;
    //重载了QHash(const T &, size_t)方法，且返回值可以和size_t转换
    template <typename T>
    constexpr inline bool HasQHashOverload<T, std::enable_if_t<
            std::is_convertible_v<decltype(qHash(std::declval<const T &>(), std::declval<size_t>())), size_t>
    >> = true;

    //重载了继承自std::hash,实现了(const T &t, size_t seed)的仿函数
    template <typename T, typename = void>
    constexpr inline bool HasStdHashSpecializationWithSeed = false;
    template <typename T>
    constexpr inline bool HasStdHashSpecializationWithSeed<T, std::enable_if_t<
            std::is_convertible_v<decltype(std::hash<T>()(std::declval<const T &>(), std::declval<size_t>())), size_t>
    >> = true;

    //重载了继承自std::hash，实现了(const T &)的仿函数
    template <typename T, typename = void>
    constexpr inline bool HasStdHashSpecializationWithoutSeed = false;
    template <typename T>
    constexpr inline bool HasStdHashSpecializationWithoutSeed<T, std::enable_if_t<
            std::is_convertible_v<decltype(std::hash<T>()(std::declval<const T &>())), size_t>
    >> = true;

    //计算hash值, 依次判断 QHash(const T, &, size_t)， std::hash<T>(const T &, size_t), std::hash<T>(const T &)
    template <typename T>
    size_t calculateHash(const T &t, size_t seed = 0) {
        if constexpr (HasQHashOverload<T>) {
            return qHash(t, seed);
        }
        else if constexpr(HasStdHashSpecializationWithSeed<T>) {
            return std::hash<T>()(t, seed);
        }
        else if constexpr (HasStdHashSpecializationWithoutSeed<T>) {
            return std::hash<T>()(t);
        }
        else {
            static_assert(sizeof(T) == 0, "The key type must have a qHash overload or a std::hash specialization");
            return 0;
        }
    }
}

namespace QHashPrivate
{
    //Node为存放键值对的节点，Node通过Span管理，Node的内存在堆中。Span中存放的Node的指针
    template <typename Key, typename T>
    struct Node {
        using KeyType = Key;
        using ValueType = T;

        Key key;
        T value;

        template <typename ...Args>
        static void createInPlace(Node *n, Key &&key, Args &&...args) {
            new (n) Node { std::move(key), T(std::forward<Args>(args)...) };
        }

        template <typename ...Args>
        static void createInPlace(Node *n, const Key &key, Args &&... args) {
            new (n) Node { Key(key), T(std::forward<Args>(args)...) };
        }

        template <typename ...Args>
        void emplaceValue(Args &&... args) {
            value = T(std::forward<Args>(args)...);
        }

        T &&takeValue() noexcept {
            //zhaoyujie TODO 为什么会有takeValue存在，且返回的是T &&
            return std::move(value);
        }

        bool valuesEqual(const Node *other) const {
            return value == other->value;
        }
    };

    //hash的节点
    template <typename Key>
    struct Node<Key, QHashDummyValue> {
        using KeyType = Key;
        using ValueType = QHashDummyValue;
        Key key;

        template <typename ...Args>
        static void createInPlace(Node *n, Key &&key, Args &&...) {
            new (n) Node{ std::move(key) };
        }

        template <typename  ...Args>
        static void createInPlace(Node *n, const Key &key, Args &&...) {
            new (n) Node{ key };
        }

        template <typename ...Args>
        void emplaceValue(Args &&...) {

        }
        ValueType TakeValue() { return QHashDummyValue(); }
        bool valuesEqual(const Node *) { return true; }
    };

    //multiHash节点链中的链表节点
    template <typename T>
    struct MultiNodeChain {
        T value;
        MultiNodeChain *next = nullptr;

        ~MultiNodeChain() = default;
        //释放
        qsizetype free() {
            qsizetype count = 0;
            MultiNodeChain *e = this;
            while (e) {
                MultiNodeChain *n = e->next;
                ++count;
                delete e;
                e = n;
            }
            return count;
        }
        //包含
        bool contains(const T &val) const noexcept  {
            const MultiNodeChain *e = this;
            while (e) {
                if (e->value == val) {
                    return true;
                }
                e = e->next;
            }
            return false;
        }
    };

    //包含Key和值的节点
    template <typename Key, typename T>
    struct MultiNode {
        using KeyType = Key;
        using ValueType = T;
        using Chain = MultiNodeChain<T>;

        Key key;
        Chain *value;

        MultiNode(const Key &k, Chain *chain)
            : key(k), value(chain)
        {}

        MultiNode(Key &&k, Chain *chain) noexcept
            : key(std::move(k)), value(chain)
        {}

        MultiNode(MultiNode &&other)
            : key(other.key), value(qExchange(other.value, nullptr))
        {}

        MultiNode(const MultiNode &other)
            : key(other.key)
        {
            //复制value，也需要赋值value后面跟着的节点
            Chain *c = other.value;
            Chain **e = &value;
            while (c) {
                Chain *chain = new Chain{ c->value, nullptr };
                *e = chain;
                e = &chain->next;
                c = c->next;
            }
        }

        ~MultiNode() {
            if (value) {
                value->free();
                value = nullptr;
            }
        }

        template <typename ...Args>
        static void createInPlace(MultiNode *n, Key &&key, Args &&... args) {
            new (n) MultiNode(std::move(key), new Chain{ T(std::forward<Args>(args)...), nullptr });
        }

        template <typename ...Args>
        static void createInPlace(MultiNode *n, const Key &key, Args &&... args) {
            new (n) MultiNode(key, new Chain{ T(std::forward<Args>(args)...), nullptr });
        }

        static qsizetype freeChain(MultiNode *n) noexcept  {
            qsizetype size = n->value->free();
            n->value = nullptr;
            return size;
        }

        //Args是T的构造函数的参数
        template <typename ...Args>
        void insertMulti(Args &&... args) {
            Chain *e = new Chain { T(std::forward<Args>(args)...), nullptr };
            e->next = value;
            value = e;
        }

        template <typename ...Args>
        void emplaceValue(Args &&... args) {
            value->value = T(std::forward<Args>(args)...);
        }
    };

    template <typename Node>
    constexpr bool isRelocatable() {
        return QTypeInfo<typename Node::KeyType>::isRelocatable && QTypeInfo<typename Node::ValueType>::isRelocatable;
    }

    //常规的hash表使用一系列的buckets来存储节点，分配一系列的buckets非常浪费内存。避免内存浪费，将筒分割成了一系列的Span
    template <typename Node>
    struct Span {
        enum {
            NEntries = 128,  //刚好unsigned char 能表示的最大值
            LocalBucketMask = (NEntries - 1),
            UnusedEntry = 0xff
        };
        static_assert((NEntries & LocalBucketMask) == 0);

        struct Entry {
            //nextFree和node共享同一内存，两者不能同时出现。需要调用方删除 / 添加的时候作维护
            //创造大小为sizeof(Node),对齐为alignof(Node)的内存storage
            typename std::aligned_storage<sizeof(Node), alignof(Node)>::type storage;

            unsigned char &nextFree() { return *reinterpret_cast<unsigned char *>(&storage); }
            Node &node() { return *reinterpret_cast<Node *>(&storage); }
        };

        unsigned char offsets[NEntries];  //offset里保存的是数据entries数组中的索引
        Entry *entries = nullptr;  //entries在哪里初始化的？
        unsigned char allocated = 0;  //unsigned char 刚好 128
        unsigned char nextFree = 0;

        Span() noexcept {
            memset(offsets, UnusedEntry, sizeof(offsets));
        }
        ~Span() {
            freeData();
        }
        void freeData() noexcept {
            if (entries) {
                if constexpr (!std::is_trivially_destructible<Node>::value) {
                    for (auto o : offsets) {
                        if (o != UnusedEntry) {
                            entries[o].node().~Node();  //先释放node的内存
                        }
                    }
                }
                delete []entries;  //释放entries的内存
                entries = nullptr;
            }
        }

        bool hasNode(int index) const {
            Q_ASSERT(index >= 0 && index < NEntries);
            return offsets[index] != UnusedEntry;
        }

        Node &at(int index) const noexcept {
            Q_ASSERT(index <= NEntries);
            Q_ASSERT(offsets[index] != UnusedEntry);
            return entries[offsets[index]].node();
        }

        Node *insert(size_t i) {
            Q_ASSERT(i <= NEntries);
            Q_ASSERT(offsets[i] == UnusedEntry);
            if (nextFree == allocated) {
                addStorage();  //增加存储空间
            }
            unsigned char entry = nextFree;
            Q_ASSERT(entry < allocated);
            nextFree = entries[entry].nextFree();  //当前node已经被使用了，所以将nextFrame调整为node的nextFrame。node的nextFrame指向下一个node
            offsets[i] = entry;
            return &entries[entry].node();
        }

        void addStorage() {
            Q_ASSERT(allocated < NEntries);
            Q_ASSERT(nextFree == allocated);
            const size_t increment = NEntries / 8;  //计算增长，重新分配内存
            size_t alloc = allocated + increment;
            if (alloc > NEntries) {
                Q_ASSERT(false); //zhaoyujie TODO 超过了128怎么办，增加Span？
            }
            Entry *newEntries = new Entry[alloc];

            if constexpr (isRelocatable<Node>()) {
                if (allocated) {
                    memcpy(newEntries, entries, allocated * sizeof(Entry));
                }
            }
            else {
                for (size_t i = 0; i < allocated; ++i) {
                    new (&newEntries[i].node()) Node(std::move(entries[i].node()));
                    entries[i].node().~Node();
                }
            }
            for (size_t i = allocated; i < allocated + increment; ++i) {
                newEntries[i].nextFree() = uchar(i + 1);
            }
            delete []entries;
            entries = newEntries;
            allocated = uchar(alloc);
        }

        size_t offset(int index) const noexcept {
            return offsets[index];
        }

        Node &atOffset(size_t o) noexcept {
            Q_ASSERT(o < allocated);
            return entries[o].node();
        }

        //删除
        void erase(size_t bucket) noexcept {
            Q_ASSERT(bucket <= NEntries);
            Q_ASSERT(offsets[bucket] != UnusedEntry);
            unsigned char entry = offsets[bucket];
            offsets[bucket] = UnusedEntry;

            entries[entry].node().~Node();
            entries[entry].nextFree() = nextFree;
            nextFree = entry;
        }

        //span内移动，只需要维护索引
        void moveLocal(size_t from, size_t to) noexcept {
            Q_ASSERT(offsets[from] != UnusedEntry);
            Q_ASSERT(offsets[to] == UnusedEntry);
            offsets[to] = offsets[from];
            offsets[from] = UnusedEntry;
        }

        //span间的移动，需要移动内存
        void moveFromSpan(Span &fromSpan, size_t fromIndex, size_t to) noexcept {
            Q_ASSERT(to < NEntries);
            Q_ASSERT(offsets[to] == UnusedEntry);
            Q_ASSERT(fromIndex < NEntries);
            Q_ASSERT(fromSpan.offsets[fromIndex] != UnusedEntry);
            if (nextFree == allocated) {  //需要分配内存
                addStorage();
            }
            Q_ASSERT(nextFree < allocated);
            offsets[to] = nextFree;
            Entry &toEntry = entries[nextFree];
            nextFree = toEntry.nextFree();

            size_t fromOffset = fromSpan.offsets[fromIndex];
            fromSpan.offsets[fromIndex] = UnusedEntry;
            Entry &fromEntry = fromSpan.entries[fromOffset];

            if constexpr (isRelocatable<Node>()) {
                memcpy(&toEntry, &fromEntry, sizeof(Entry));
            }
            else {
                new (&toEntry.node()) Node(std::move(fromEntry.node()));
                fromEntry.node().~Node();
            }
            fromEntry.nextFree() = fromSpan.nextFree;
            fromSpan.nextFree = static_cast<unsigned char>(fromOffset);
        }
    };

    template <typename Node>
    struct iterator;

    template <typename Node>
    struct Data {
        using Key = typename Node::KeyType;
        using T = typename Node::ValueType;
        using Span = QHashPrivate::Span<Node>;
        using iterator = QHashPrivate::iterator<Node>;

        QtPrivate::RefCount ref = {{1}};
        size_t size = 0;
        size_t numBuckets = 0; //桶的个数
        size_t seed = 0;  //hash种子

        Span *spans = nullptr;

        Data(size_t reserve = 0) {
            //计算需要的桶的数量
            numBuckets = GrowthPolicy::bucketsForCapacity(reserve);
            //计算span的数量，128个桶构成1个Span
            size_t nSpans = (numBuckets + Span::LocalBucketMask) / Span::NEntries;
            //分配Span的内存
            spans = new Span[nSpans];
            //初始化hash种子
            seed = QHashSeed::globalSeed();
//            std::cout<<"Data Seed : "<<seed<<std::endl;
        }

        Data(const Data &other, size_t reserved = 0)
                : size(other.size), numBuckets(other.numBuckets), seed(other.seed) {
            if (reserved) {
                numBuckets = GrowthPolicy::bucketsForCapacity(qMax(size, reserved));
            }
            bool resized = numBuckets != other.numBuckets;
            size_t nSpans = (numBuckets + Span::LocalBucketMask) / Span::NEntries;
            spans = new Span[nSpans];
            //复制数据
            for (size_t s = 0; s < nSpans; ++s) {
                const Span &span = other.spans[s];
                for (size_t index = 0; index < Span::NEntries; ++index) {
                    if (!span.hasNode(index)) {
                        continue;
                    }
                    const Node &n = span.at(index);
                    //如果大小没变，直接安放在原来的位置，如果大小变了，需要重新查找位置
                    iterator it = resized ? find(n.key) : iterator{this, s * Span::NEntries + index};
                    Q_ASSERT(it.isUnused());
                    Node *newNode = spans[it.span()].insert(it.index());
                    new(newNode) Node(n);
                }
            }
        }

        static Data *detached(Data *d, size_t size = 0) {
            if (!d) {
                return new Data(size); //数据为空，new出数据
            }
            Data *dd = new Data(*d, size);  //拷贝构造
            //detach之后，原先的ref需要-1。如果-1后发现没有被引用得了，删除
            //如果只有当前引用，不需要重新new了应该。代码存疑
            if (!d->ref.deref()) {
                delete d;
            }
            return dd;
        }

        iterator detachedIterator(iterator other) const noexcept {
            return iterator{this, other.bucket};
        }

        void clear() {
            delete[] spans;
            spans = nullptr;
            size = 0;
            numBuckets = 0;
        }

        ~Data() {
            delete[]spans;
        }

        iterator begin() const noexcept {
            iterator it{this, 0};  //桶的编号为0，如果0没有被使用，找到下一个被使用的桶
            if (it.isUnused()) {
                ++it;
            }
            return it;
        }

        constexpr iterator end() const noexcept {
            return iterator();
        }

        iterator erase(iterator it) noexcept {
            /*
             * 删除node，需要将之前因为碰撞检测不通过而插入在后面的node移动到正确的位置
             * 针对下一个node，情况有三种：
             * 1. 计算的位置就是实际位置，位置摆放正确，不需要操作
             * 2. 计算的位置是空出来的位置，需要移动node，并循环移动后面的数据
             * 3. 计算的位置不是上面两个位置，这说明是插入时，该插入的位置已经被占用了，不得不往后插入
             *    3.1 洞可能留在插入过程中已经被判断了，被占用了的位置上，需要将Node移动到洞中，同情形2
             *    3.2 也可能插入的过程中还没有判断到洞的位置，同情形1
             */
            size_t bucket = it.bucket;
            size_t span = bucket / Span::NEntries;
            size_t index = bucket & Span::LocalBucketMask;
            Q_ASSERT(spans[span].hasNode(index));
            spans[span].erase(index);  //从span中删除了Node，且重置了offset和nextFree
            --size;

            size_t hole = bucket;  //留下的空洞
            size_t next = bucket;
            while (true) {
                next = nextBucket(next); //查找下一个node
                size_t nextSpan = next / Span::NEntries;
                size_t nextIndex = next & Span::LocalBucketMask;
                if (!spans[nextSpan].hasNode(nextIndex)) {  //如果Node不存在，跳出循环
                    break;
                }
                size_t hash = QHashPrivate::calculateHash(spans[nextSpan].at(nextIndex).key, seed);
                size_t newBucket = GrowthPolicy::bucketForHash(numBuckets, hash);   //计算next应该处在的位置
                while (true) {
                    if (newBucket == next) {   //摆在正确的位置，跳出循环，继续检测下一个
                        break;
                    }
                    else if (newBucket == hole) {  //位置在留下的洞处，将Node移过来，继续移动后面的node
                        size_t holeSpan = hole / Span::NEntries;
                        size_t holeIndex = hole & Span::LocalBucketMask;
                        if (nextSpan == holeSpan) {
                            //在同一个span内部移动
                            spans[holeSpan].moveLocal(nextIndex, holeIndex);
                        }
                        else {
                            //跨span移动
                            spans[holeSpan].moveFromSpan(spans[nextSpan], nextIndex, holeIndex);
                        }
                        hole = next;
                        break;
                    }
                    newBucket = nextBucket(newBucket);
                }
            }
            if (bucket == numBuckets - 1 || !spans[span].hasNode(index)) {
                ++it;
            }
            return it;
        }

        iterator find(const Key &key) const noexcept {
            Q_ASSERT(numBuckets > 0);
            //找到key对应的hash
            size_t hash = QHashPrivate::calculateHash(key, seed);  //计算hash值
            size_t bucket = GrowthPolicy::bucketForHash(numBuckets, hash);  //计算bucket索引
            while (true) {
                size_t span = bucket / Span::NEntries;  //获取span
                size_t index = bucket & Span::LocalBucketMask;  //获取在span中存放的索引
                Span &s = spans[span];
                size_t offset = s.offset(index);
                if (offset == Span::UnusedEntry) {
                    return iterator{this, bucket};  //key对应的hash指定位置没有被使用
                } else {
                    Node &n = s.atOffset(offset);
                    if (qHashEquals(n.key, key)) {  //key对应的hash制定位置被使用了，并且key相等
                        return iterator{this, bucket};
                    }
                }
                //循环使用后面的bucket，这个操作会占用其他hash的空间
                bucket = nextBucket(bucket);
            }
        }

        size_t nextBucket(size_t bucket) const noexcept {
            ++bucket;  //下一个桶子。桶子能不能被使用，调用方判断
            if (bucket == numBuckets) {
                bucket = 0;
            }
            return bucket;
        }

        Node *findNode(const Key &key) const noexcept {
            if (!size) {
                return nullptr;
            }

            iterator it = find(key);
            if (it.isUnused()) {
                return nullptr;
            }
            return it.node();
        }

        struct InsertionResult {
            iterator it;
            bool initialized;
        };

        //查找或者插入
        InsertionResult findOrInsert(const Key &key) noexcept {
            if (shouldGrow()) {   //超过负载重新分配空间
                rehash(size + 1);
            }
            iterator it = find(key);  //查找到的位置没有被使用
            if (it.isUnused()) {
                spans[it.span()].insert(it.index());  //在相应位置插入
                ++size;
                return { it, false };
            }
            return { it, true };
        }

        //附在读
        float loadFactor() const noexcept  {
            return float(size) / numBuckets;
        }

        //负载超过一半时，重新分配空间。负载过大会劣化效率
        bool shouldGrow() const noexcept {
            return size >= (numBuckets >> 1);
        }

        void rehash(size_t sizeHint = 0) {
            if (sizeHint == 0) {
                sizeHint = size;
            }
            size_t newBucketCount = GrowthPolicy::bucketsForCapacity(sizeHint);
            Span *oldSpans = spans;
            size_t oldBucketCount = numBuckets;
            size_t nSpans = (newBucketCount + Span::LocalBucketMask) / Span::NEntries;
            spans = new Span[nSpans];
            numBuckets = newBucketCount;
            size_t oldNSpans = (oldBucketCount + Span::LocalBucketMask) / Span::NEntries;

            for (size_t s = 0; s < oldNSpans; ++s) {
                Span &span = oldSpans[s];
                for (size_t index = 0; index < Span::NEntries; ++index) {
                    if (!span.hasNode(index)) {
                        continue;
                    }
                    Node &n = span.at(index);
                    iterator it = find(n.key);
                    Q_ASSERT(it.isUnused());
                    Node *newNode = spans[it.span()].insert(it.index());
                    new (newNode) Node(std::move(n));
                }
                span.freeData();
            }
            delete []oldSpans;
        }
    };

    //Span的迭代器
    template <typename Node>
    struct iterator {
        using Span = QHashPrivate::Span<Node>;
        const Data<Node> *d = nullptr;
        size_t bucket = 0;

        size_t span() const noexcept { return bucket / Span::NEntries; }
        size_t index() const noexcept { return bucket & Span::LocalBucketMask; }

        inline bool isUnused() const noexcept { return !d->spans[span()].hasNode(index()); }

        inline Node *node() const noexcept  {
            Q_ASSERT(!isUnused());
            return &d->spans[span()].at(index());
        }

        //迭代++ / 初始化，如果是end，会将d设置为nullptr
        bool atEnd() const noexcept { return !d; }

        iterator operator++() noexcept  {
            //找到下一个被使用的对象，找不到就设置为end
            while (true) {
                ++bucket;
                if (bucket == d->numBuckets) {  //到了末尾了,将d设置成nullptr，桶编号设置成0作为end
                    d = nullptr;
                    bucket = 0;
                    break;
                }
                if (!isUnused()) {
                    break;
                }
            }
            return *this;
        }
        //没有i++的迭代器

        bool operator==(iterator other) const noexcept  {
            return d == other.d && bucket == other.bucket;
        }

        bool operator!=(iterator other) const noexcept  {
            return !(*this == other);
        }
    };
}

template <typename Key, typename T>
class QHash
{
    using Node = QHashPrivate::Node<Key, T>;
    using Data = QHashPrivate::Data<Node>;
//    friend class QSet<Key>;
    friend class QMultiHash<Key, T>;
    friend tst_QHash;

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = T;
    using size_type = qsizetype ;
    using difference_type = qsizetype ;
    using reference = T &;
    using const_reference = const T &;

    inline QHash() noexcept = default;

    inline QHash(std::initializer_list<std::pair<Key, T>> list)
        : d(new Data(list.size())) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            insert(it->first, it->second);
        }
    }

    QHash(const QHash &other) noexcept
        : d(other.d) {
        if (d) {
            d->ref.ref();
        }
    }

    ~QHash() {
        if (d && !d->ref.deref()) {
            delete d;
        }
    }

    QHash &operator=(const QHash<Key, T> &other) noexcept {
        if (d != other.d) {
            Data *o = other.d;
            if (o) {
                o->ref.ref();
            }
            if (d && !d->ref.deref()) {
                delete d;
            }
            d = o;
        }
        return *this;
    }

    QHash(QHash &&other) noexcept
        : d(std::exchange(other.d, nullptr)) {

    }

    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QHash)

    //通过Qt风格迭代器构造
    template <typename InputIterator, QtPrivate::IfAssociativeIteratorHasKeyAndValue<InputIterator> = true>
    QHash(InputIterator f, InputIterator l)
        : QHash() {
        //reserve容量
        QtPrivate::reserveIfForwardIterator(this, f, l);
        for (; f != l; ++f) {
            insert(f.key(), f.value());
        }
    }
    //通过stl风格迭代器构造
    template <typename InputIterator, QtPrivate::IfAssociativeIteratorHasFirstAndSecond<InputIterator> = true>
    QHash(InputIterator f, InputIterator l)
        : QHash() {
        QtPrivate::reserveIfForwardIterator(this, f, l);
        for (; f != l; ++f) {
            insert(f.first, f.second);
        }
    }

    void swap(QHash &other) noexcept {
        qSwap(d, other.d);
    }

    template <typename U = T>
    QTypeTraits::compare_eq_result_container<QHash, U> operator==(const QHash &other) const noexcept {
        if (d == other.d) {
            return true;
        }
        if (size() != other.size()) {
            return false;
        }
        for (const_iterator it = other.begin(); it != other.end(); ++it) {
            const_iterator i = find(it.key());
            if (i == end() || !i.i.node()->valuesEqual(it.i.node())) {
                return false;
            }
        }
        return true;
    }

    template <typename U = T>
    QTypeTraits::compare_eq_result_container<QHash, U> operator!=(const QHash &other) const noexcept {
        return !(*this == other);
    }

    inline qsizetype size() const noexcept { return d ? qsizetype(d->size) : 0; }
    inline bool isEmpty() const noexcept { return size() == 0; }

    //有负载因子的存在避免碰撞，负载因子最大为0.5，所以右移一位
    inline qsizetype capacity() const noexcept  { return d ? qsizetype(d->numBuckets >> 1) : 0; }
    void reserve(qsizetype size) {
        if (isDetached()) {  //size比当前的元素个数少时，在rehash里复制数据的时候，增加了内存
            d->rehash(size);
        }
        else {
            d = Data::detached(d, size_t(size));
        }
    }

    inline void squeeze() {
        if (capacity()) {
            reserve(0);
        }
    }

    inline void detach() {
        if (!d || d->ref.isShared()) {
            d = Data::detached(d);
        }
    }

    inline bool isDetached() const noexcept  { return d && !d->ref.isShared(); }
    bool isSharedWith(const QHash &other) const { return d == other.d; }

    void clear() noexcept {
        if (d && !d->ref.deref()) {  //如果只有当前持有，需要删除
            delete d;
        }
        d = nullptr;  //非当前持有，直接设置为nullptr，引用计数在上面已经做过了deref操作
    }

    bool remove(const Key &key) {
        if (isEmpty()) {
            return false;
        }
        detach();
        auto it = d->find(key);
        if (it.isUnused()) {
            return false;
        }
        d->erase(it);
        return true;
    }

    template <typename Predicate>
    qsizetype removeIf(Predicate pred) {
        return QtPrivate::associative_erase_if(*this, pred);
    }

    T take(const Key &key) {
        if (isEmpty()) {
            return T();
        }
        detach();
        auto it = d->find(key);
        if (it.isUnused()) {
            return T();
        }
        T value = it.node()->takeValue();
        d->erase(it);
        return value;
    }

    bool contains(const Key &key) const noexcept  {
        if (!d) {
            return false;
        }
        return d->findNode(key) != nullptr;
    }

    qsizetype count(const Key &key) const noexcept  {
        return contains(key) ? 1 : 0;
    }

    Key key(const T &value, const Key &defaultKey = Key()) const noexcept {
        if (d) {
            const_iterator i = begin();
            while (i != end()) {
                if (i.value() == value) {
                    return i.key();
                }
                ++i;
            }
        }
        return defaultKey;
    }

    T value(const Key &key, const T &defaultValue = T()) const noexcept  {
        if (d) {
            Node *n = d->findNode(key);
            if (n) {
                return n->value;
            }
        }
        return defaultValue;
    }

    T &operator[](const Key &key) {
        detach();
        auto result = d->findOrInsert(key);
        Q_ASSERT(!result.it.atEnd());
        if (!result.initialized) {  //没有的话就创造一个
            Node::createInPlace(result.it.node(), key, T());
        }
        return result.it.node()->value;
    }

    const T operator[](const Key &key) const noexcept  {
        return value(key);
    }

    QList<Key> keys() const { return QList<Key>(keyBegin(), keyEnd()); }
    QList<Key> keys(const T &value) const {
        QList<Key> res;
        const_iterator i = begin();
        while (i != end()) {
            if (i.value() == value) {
                res.append(i.key());
            }
            ++i;
        }
        return res;
    }

    QList<T> values() const { return QList<T>(begin(), end()); }

    class const_iterator;

    class iterator {
        using piter = typename QHashPrivate::iterator<Node>;
        friend class const_iterator;
        friend class QHash<Key, T>;
        friend class QSet<Key>;
        piter i;   //内置了Spin的iterator
        explicit inline iterator(piter it) noexcept : i(it) {}

    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        constexpr iterator() noexcept = default;

        inline const Key &key() const noexcept { return i.node()->key; }
        inline T &value() const noexcept { return i.node()->value; }
        inline T &operator*() const noexcept  { return i.node()->value; }
        inline T *operator->() const noexcept  { return &i.node()->value; }
        inline bool operator==(const iterator &other) const noexcept { return i == other.i; }
        inline bool operator!=(const iterator &other) const noexcept { return i != other.i; }

        inline iterator &operator++() noexcept  {
            ++i;
            return *this;
        }
        inline iterator operator++(int) noexcept {
            iterator r = *this;
            ++i;
            return r;
        }

        inline bool operator==(const const_iterator &other) const noexcept { return i == other.i; }
        inline bool operator!=(const const_iterator &other) const noexcept { return i != other.i; }
    };
    friend class iterator;

    class const_iterator {
        using piter = typename QHashPrivate::iterator<Node>;
        friend class iterator;
        friend class QHash<Key, T>;
        friend class QSet<Key>;
        piter i;
        explicit inline const_iterator(piter it) : i(it) { }

    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        constexpr const_iterator() noexcept = default;
        inline const_iterator(const iterator &o) noexcept : i(o.i) {}

        inline const Key &key() const noexcept  { return i.node()->key; }
        inline const T &value() const noexcept  { return i.node()->value; }
        inline const T &operator*() const noexcept  { return i.node()->value; }
        inline const T *operator->() const noexcept  { return &i.node()->value; }

        inline bool operator==(const const_iterator &o) const noexcept  { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const noexcept  { return i != o.i; }

        inline const_iterator &operator++() noexcept {
            ++i;
            return *this;
        }
        inline const_iterator operator++(int) {
            const_iterator r = *this;
            ++i;
            return r;
        }
    };

    friend class const_iterator;

    //key_iterator使用的是const_iterator
    class key_iterator {
        const_iterator i;

    public:
        typedef typename const_iterator::iterator_category iterator_category;
        typedef qptrdiff difference_type;
        typedef Key value_type;
        typedef Key *pointer;
        typedef Key &reference;

        key_iterator() noexcept = default;
        explicit key_iterator(const_iterator o) noexcept  : i(o) {}
        const Key &operator*() const noexcept  { return i.key(); }
        const Key *operator->() const noexcept { return &i.key(); }
        bool operator==(key_iterator o) const noexcept { return i == o.i; }
        bool operator!=(key_iterator o) const noexcept { return i != o.i; }

        inline key_iterator &operator++() noexcept  { ++i; return *this; }
        inline key_iterator operator++(int) noexcept {
            key_iterator r = *this;
            ++i;
            return r;
        }

        const_iterator base() const noexcept { return i; }
    };

    typedef QKeyValueIterator<const Key &, const T &, const_iterator> const_key_value_iterator;
    typedef QKeyValueIterator<const Key &, T &, iterator> key_value_iterator;

    //STL风格
    inline iterator begin() { detach(); return iterator(d->begin()); }
    inline const_iterator begin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }
    inline const_iterator cbegin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }
    inline const_iterator constBegin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }

    inline iterator end() { return iterator(); }  //iterator默认为end的状态
    inline const_iterator end() const noexcept { return const_iterator(); }
    inline const_iterator cend() const noexcept { return const_iterator(); }
    inline const_iterator constEnd() const noexcept  { return const_iterator(); }

    inline key_iterator keyBegin() const noexcept  { return key_iterator(begin()); }
    inline key_iterator keyEnd() const noexcept { return key_iterator(end()); }
    inline key_value_iterator keyValueBegin() { return key_value_iterator(begin()); }
    inline key_value_iterator keyValueEnd() { return key_value_iterator(end()); }
    inline const_key_value_iterator keyValueBegin() const noexcept  { return const_key_value_iterator(begin()); }
    inline const_key_value_iterator constKeyValueBegin() const noexcept { return const_key_value_iterator(begin()); }
    inline const_key_value_iterator keyValueEnd() const noexcept  { return const_key_value_iterator(end()); }
    inline const_key_value_iterator constKeyValueEnd() const noexcept  { return const_key_value_iterator(end()); }

    iterator erase(const_iterator it) {
        Q_ASSERT(it != constEnd());
        detach();
        iterator i = iterator{ d->detachedIterator(it.i) };  //复制迭代器
        i.i = d->erase(i.i);  //删除
        return i;
    }

    QPair<iterator, iterator> equal_range(const Key &key) {
        auto first = find(key);
        auto second = first;
        if (second != iterator()) {
            ++second;
        }
        return qMakePair(first, second);
    }

    QPair<const_iterator, const_iterator> equal_range(const Key &key) const {
        auto first = find(key);
        auto second = first;
        if (second != iterator()) {
            ++second;
        }
        return qMakePair(first, second);
    }

    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    inline qsizetype count() const { return d ? d->size : 0; }
    iterator find(const Key &key) {
        if (isEmpty()) {
            return end();
        }
        detach();
        auto it = d->find(key);
        if (it.isUnused()) {
            it = d->end();
        }
        return iterator(it);
    }

    const_iterator find(const Key &key) const noexcept  {
        if (isEmpty()) {
            return end();
        }
        auto it = d->find(key);
        if (it.isUnused()) {
            it = d->end();
        }
        return const_iterator(it);
    }

    const_iterator constFind(const Key &key) const noexcept {
        return find(key);
    }

    iterator insert(const Key &key, const T &value) {
        return emplace(key, value);
    }
    void insert(const QHash &hash) {
        if (d == hash.d || !hash.d) {
            return;
        }
        if (!d) {
            *this = hash;
            return;
        }
        detach();
        for (auto it = hash.begin(); it != hash.end(); ++it) {
            emplace(it.key(), it.value());
        }
    }

    template <typename ...Args>
    iterator emplace(const Key &key, Args &&...args) {
        Key copy = key;
        return emplace(std::move(copy), std::forward<Args>(args)...);
    }

    template <typename ...Args>
    iterator emplace(Key &&key, Args &&...args) {
        detach();
        auto result = d->findOrInsert(key);  //result中已经分配了Node
        if (!result.initialized) {
            Node::createInPlace(result.it.node(), std::move(key), std::forward<Args>(args)...);
        }
        else {
            result.it.node()->emplaceValue(std::forward<Args>(args)...);
        }
        return iterator(result.it);
    }

    float load_factor() const noexcept  { return d ? d->loadFactor() : 0; }
    static float max_load_factor() noexcept { return 0.5; }
    size_t bucket_count() const noexcept  { return d ? d->numBuckets : 0; }
    static size_t max_bucket_count() noexcept  { Q_ASSERT(false); return QHashPrivate::GrowthPolicy::maxNumBuckets(); }
    inline bool empty() const noexcept  { return isEmpty(); }

private:
    Data *d = nullptr;
};

template <typename Key, typename T>
class QMultiHash
{
    using Node = QHashPrivate::MultiNode<Key, T>;
    using Data = QHashPrivate::Data<Node>;
    using Chain = QHashPrivate::MultiNodeChain<T>;

    Data *d = nullptr;
    qsizetype m_size = 0;

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = T;
    using size_type = qsizetype;
    using difference_type = qsizetype ;
    using reference = T &;
    using const_reference = const T &;

    QMultiHash() noexcept = default;
    inline QMultiHash(std::initializer_list<std::pair<Key, T>> list)
        : d(new Data(list.size()))
    {
        for (auto it = list.begin(); it != list.end(); ++it) {
            insert(it->first, it->second);
        }
    }

    template <typename InputIterator, QtPrivate::IfAssociativeIteratorHasKeyAndValue<InputIterator> = true>
    QMultiHash(InputIterator f, InputIterator l)
    {
        QtPrivate::reserveIfForwardIterator(this, f, l);
        for (; f != l; ++f) {
            insert(f.key(), f.value());
        }
    }

    template <typename InputIterator, QtPrivate::IfAssociativeIteratorHasFirstAndSecond<InputIterator> = true>
    QMultiHash(InputIterator f, InputIterator l)
    {
        QtPrivate::reserveIfForwardIterator(this, f, l);
        for (; f != l; ++f) {
            insert(f->first, f->second);
        }
    }

    QMultiHash(const QMultiHash &other) noexcept
        : d(other.d), m_size(other.m_size)
    {
        if (d) {
            d->ref.ref();
        }
    }

    ~QMultiHash() {
        if (d && !d->ref.deref()) {
            delete d;
        }
    }

    QMultiHash &operator=(const QMultiHash &other) noexcept {
        if (d != other.d) {
            Data *o = other.d;
            if (o) {
                o->ref.ref();
            }
            if (d && !d->ref.deref()) {
                delete d;
            }
            d = o;
            m_size = other.m_size;
        }
        return *this;
    }

    QMultiHash(QMultiHash &&other) noexcept
        : d(qExchange(other.d, nullptr))
        , m_size(qExchange(other.m_size, 0))
    {
    }

    QMultiHash &operator=(QMultiHash &&other) noexcept {
        QMultiHash moved(std::move(other));
        swap(moved);
        return *this;
    }

    explicit QMultiHash(const QHash<Key, T> &other)
        : QMultiHash(other.begin(), other.end())
    {
    }

    explicit QMultiHash(QHash<Key, T> &&other)
    {
        unite(std::move(other));
    }

    void swap(QMultiHash &other) noexcept {
        qSwap(d, other.d);
        qSwap(m_size, other.m_size);
    }

    bool operator==(const QMultiHash &other) const noexcept {
        if (d == other.d) {
            return true;
        }
        if (m_size != other.m_size) {
            return false;
        }
        if (m_size == 0) {
            return true;
        }
        Q_ASSERT(d && other.d);

        if (d->size != other.d->size) {
            return false;
        }
        for (auto it = other.d->begin(); it != other.d->end(); ++it) {
            auto i = d->find(it.node()->key);
            if (i == d->end()) {
                return false;
            }
            Chain *e = it.node()->value;
            while (e) {
                Chain *oe = i.node()->value;
                while (oe) {
                    if (oe->value == e->value) {
                        break;
                    }
                    oe = oe->next;
                }

                if (!oe) {
                    return false;
                }
                e = e->next;
            }
        }
        return true;
    }

    bool operator!=(const QMultiHash &other) const noexcept { return !(*this == other); }

    inline qsizetype size() const noexcept  { return m_size; }

    inline bool isEmpty() const noexcept  { return !m_size; }

    inline qsizetype capacity() const noexcept { return d ? qsizetype(d->numBuckets >> 1) : 0; }

    void reserve(qsizetype size) {
        if (isDetached()) {
            d->rehash(size);
        }
        else {
            d = Data::detached(d, size_t(size));
        }
    }

    inline void squeeze() { reserve(0); }

    inline void detach() {
        if (!d || d->ref.isShared()) {  //d为空，detached里new出数据。d被共享。detached内进行复制
            d = Data::detached(d);
        }
    }
    inline bool isDetached() const noexcept  { return d && !d->ref.isShared(); }

    bool isSharedWith(const QMultiHash &other) const noexcept  { return d == other.d; }

    void clear() noexcept {
        if (d && !d->ref.deref()) {
            delete d;
        }
        d = nullptr;
        m_size = 0;
    }

    qsizetype remove(const Key &key)
    {
        if (isEmpty()) {
            return 0;
        }
        detach();

        auto it = d->find(key);
        if (it.isUnused()) {
            return 0;
        }
        qsizetype n = Node::freeChain(it.node());
        m_size -= n;
        Q_ASSERT(m_size >= 0);
        d->erase(it);
        return n;
    }

    template <typename Predicate>
    qsizetype removeIf(Predicate pred) {
        return QtPrivate::associative_erase_if(*this, pred);
    }

    T take(const Key &key)
    {
        if (isEmpty()) {
            return T();
        }
        detach();

        auto it = d->find(key);
        if (it.isUnused()) {
            return T();
        }
        Chain *e = it.node()->value;
        Q_ASSERT(e);
        T t = std::move(e->value);
        if (e->next) {
            it.node()->value = e->next;
            delete e;
        }
        else {
            d->erase(it);
        }
        --m_size;
        Q_ASSERT(m_size >= 0);
        return t;
    }

    bool contains(const Key &key) const noexcept {
        if (!d) {
            return false;
        }
        return d->findNode(key) != nullptr;
    }

    Key key(const T &value, const Key &defaultKey = Key()) const noexcept {
        if (d) {
            auto i = d->begin();
            while (i != d->end()) {
                Chain *e = i.node()->value;
                if (e->contains(value)) {
                    return i.node()->key;
                }
                ++i;
            }
        }
        return defaultKey;
    }

    T value(const Key &key, const T &defaultValue = T()) const noexcept {
        if (d) {
            Node *n = d->findNode(key);
            if (n) {
                Q_ASSERT(false);
                return n->value->value;
            }
        }
        return defaultValue;
    }

    T &operator[](const Key &key) {
        detach();
        auto result = d->findOrInsert(key);
        Q_ASSERT(!result.it.atEnd());
        if (!result.initialized) {
            Node::createInPlace(result.it.node(), key, T());
        }
        return result.it.node()->value->value;
    }

    const T operator[](const Key &key) const noexcept
    {
        return value(key);
    }

    QList<Key> uniqueKeys() const
    {
        QList<Key> res;
        if (d) {
            auto i = d->begin();
            while (i != d->end()) {
                res.append(i.node()->key);
                ++i;
            }
        }
        return res;
    }

    QList<Key> keys() const {
        return QList<Key>(keyBegin(), keyEnd());
    }

    QList<Key> keys(const T &value) const {
        QList<Key> res;
        const_iterator i = begin();
        while (i != end()) {
            if (i.value() == value) {
                res.append(i.key());
            }
            ++i;
        }
        return res;
    }

    QList<T> values() const { return QList<T>(begin(), end()); }
    QList<T> values(const Key &key) const {
        QList<T> values;
        if (d) {
            Node *n = d->findNode(key);
            if (n) {
                Chain *e = n->value;
                while (e) {
                    values.append(e->value);
                    e = e->next;
                }
            }
        }
        return values;
    }

    class const_iterator;

    class iterator
    {
        using piter = typename QHashPrivate::iterator<Node>;
        friend class const_iterator;
        friend class QMultiHash<Key, T>;
        piter i;
        Chain **e = nullptr;
        explicit inline iterator(piter it, Chain **entry = nullptr) noexcept
            : i(it)
            , e(entry)
        {
            if (!it.atEnd() && !e) {
                e = &it.node()->value;
                Q_ASSERT(e && *e);
            }
        }

    public:
        typename std::forward_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        constexpr iterator() noexcept = default;

        inline const Key &key() const noexcept { return i.node()->key; }
        inline T &value() const noexcept { return (*e)->value; }
        inline T &operator*() const noexcept { return (*e)->value; }
        inline T *operator->() const noexcept { return &(*e)->value; }
        inline bool operator==(const iterator &o) const noexcept { return e == o.e; }
        inline bool operator!=(const iterator &o) const noexcept { return e != o.e; }

        inline iterator &operator++() noexcept  {
            Q_ASSERT(e && *e);
            e = &(*e)->next;
            Q_ASSERT(e);
            if (!*e) {
                ++i;
                e = i.atEnd() ? nullptr : &i.node()->value;
            }
            return *this;
        }

        inline iterator operator++(int) noexcept  {
            iterator r = *this;
            ++(*this);
            return r;
        }

        inline bool operator==(const const_iterator &o) const noexcept  { return e == o.e; }
        inline bool operator!=(const const_iterator &o) const noexcept  { return e != o.e; }
    };
    friend class iterator;

    class const_iterator{
        using piter = typename QHashPrivate::iterator<Node>;
        friend class iterator;
        friend class QMultiHash<Key, T>;
        piter i;
        Chain **e = nullptr;   //piter的迭代器粒度到Node层，所以这里需要Chain记录在链条中的位置
        explicit inline const_iterator(piter it, Chain **entry = nullptr) noexcept
            : i(it)
            , e(entry)
        {
            if (!it.atEnd() && !e) {
                e = &it.node()->value;
                Q_ASSERT(e && *e);
            }
        }

    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef const T *pinter;
        typedef const T &reference;

        constexpr const_iterator() noexcept = default;
        inline const_iterator(const iterator &o) noexcept
            : i(o.i), e(o.e)
        {}

        inline const Key &key() const noexcept  { return i.node()->key; }
        inline T &value() const noexcept { return (*e)->value; }
        inline T &operator*() const noexcept { return (*e)->value; }
        inline T *operator->() const noexcept { return &(*e)->value; }
        inline bool operator==(const const_iterator &o) const noexcept  { return e == o.e; }
        inline bool operator!=(const const_iterator &o) const noexcept  { return e != o.e; }

        inline const_iterator &operator++() noexcept {
            Q_ASSERT(e && *e);
            e = &(*e)->next;
            Q_ASSERT(e);
            if (!*e) {
                ++i;
                e = i.atEnd() ? nullptr : &i.node()->value;
            }
            return *this;
        }

        inline const_iterator operator++(int) noexcept {
            const_iterator r = *this;
            ++(*this);
            return r;
        }
    };
    friend class const_iterator;

    class key_iterator {
        const_iterator i;

    public:
        typedef typename const_iterator::iterator_category iterator_category;
        typedef qptrdiff difference_type;
        typedef Key value_type;
        typedef const Key *pointer;
        typedef const Key &reference;

        key_iterator() noexcept = default;
        explicit key_iterator(const_iterator o) noexcept : i(o) {}

        const Key &operator*() const noexcept { return i.key(); }
        const Key *operator->() const noexcept { return &i.key(); }
        bool operator==(const key_iterator &o) const { return i == o.i; }
        bool operator!=(const key_iterator &o) const { return i != o.i; }

        inline key_iterator &operator++() noexcept { ++i; return *this; }
        inline key_iterator operator++(int) noexcept { auto r = *this; ++i; return r; }
        const_iterator base() const noexcept { return i; }
    };

    typedef QKeyValueIterator<const Key &, const T &, const_iterator> const_key_value_iterator;
    typedef QKeyValueIterator<const Key &, T &, const_iterator> key_value_iterator;

    //STL 风格的接口
    inline iterator begin() { detach(); return iterator(d->begin()); }
    inline const_iterator begin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }
    inline const_iterator cbegin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }
    inline const_iterator constBegin() const noexcept { return d ? const_iterator(d->begin()) : const_iterator(); }
    inline iterator end() noexcept { return iterator(); }
    inline const_iterator end() const noexcept { return iterator(); }
    inline const_iterator cend() const noexcept { return iterator(); }
    inline const_iterator constEnd() const noexcept { return iterator(); }

    inline key_iterator keyBegin() const { return key_iterator(begin()); }
    inline key_iterator keyEnd() const { return key_iterator(end()); }

    inline key_value_iterator keyValueBegin() { return key_value_iterator(begin()); }
    inline key_value_iterator keyValueEnd() { return key_value_iterator(end()); }

    inline const_key_value_iterator keyValueBegin() const noexcept { return const_key_value_iterator(begin()); }
    inline const_key_value_iterator constKeyValueBegin() const noexcept { return const_key_value_iterator(begin()); }
    inline const_key_value_iterator keyValueEnd() const noexcept { return const_key_value_iterator(end()); }
    inline const_key_value_iterator constKeyValueEnd() const noexcept { return const_key_value_iterator(end()); }

    //对data做detach操作，同时根据迭代器的位置对detach后的数据进行迭代器的恢复
    iterator detach(const_iterator it) {
        auto i = it.i;
        Chain **e = it.e;
        if (d->ref.isShared()) {
            qsizetype n = 0;
            Chain *entry = i.node()->value;
            while (entry != *it.e) {   //存储迭代器处于ChainNode链条中的位置
                ++n;
                entry = entry->next;
            }
            Q_ASSERT(entry);
            detach_helper();  //detach data

            i = d->detachedIterator(i);  //上面进行了detach，迭代器也需要重新指向
            e = &i.node()->value;   //根据ChainNode链条中的位置恢复指针指向
            while (n) {
                e = &(*e)->next;
                --n;
            }
            Q_ASSERT(e && *e);
        }
        return iterator(i, e);
    }

    iterator erase(const_iterator it) {
        Q_ASSERT(d);
        iterator i = detach(it);
        Chain *e = *i.e;  //将迭代器i的值指向it->next
        Chain *next = e->next;
        *i.e = next;
        delete e;  //删除节点e
        if (!next) {  //next为空的话，需要从下一个ChainNode中获取下一个元素
            if (i.e == &i.i.node()->value) {
                i = iterator(d->erase(i.i));  //如果ChainNode里没有元素了，删掉这个Node
            }
            else {
                i = iterator(++it.i);  //ChainNode中还有元素，
            }
        }
        --m_size;
        Q_ASSERT(m_size >= 0);
        return i;
    }

    //Qt风格
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    inline qsizetype count() const noexcept { return size(); }
    iterator find(const Key &key) {
        if (isEmpty()) {
            return end();
        }
        detach();
        auto it = d->find(key);
        if (it.isUnused()) {
            it = d->end();
        }
        return iterator(it);
    }

    const_iterator find(const Key &key) const noexcept
    {
        return constFind(key);
    }

    const_iterator constFind(const Key &key) const noexcept {
        if (isEmpty()) {
            return end();
        }
        auto it = d->find(key);
        if (it.isUnused()) {
            it = d->end();
        }
        return const_iterator(it);
    }

    iterator insert(const Key &key, const T &value) {
        return emplace(key, value);
    }

    template <typename ...Args>
    iterator emplace(const Key &key, Args &&...args)
    {
        return emplace(Key(key), std::forward<Args>(args)...);
    }

    template <typename ...Args>
    iterator emplace(Key &&key, Args &&...args)
    {
        detach();

        auto result = d->findOrInsert(key);
        if (!result.initialized) {
            Node::createInPlace(result.it.node(), std::move(key), std::forward<Args>(args)...);
        }
        else {
            result.it.node()->insertMulti(std::forward<Args>(args)...);
        }
        ++m_size;
        return iterator(result.it);
    }

    float load_factor() const noexcept { return d ? d->loadFactor() : 0; }
    static float max_load_factor() noexcept  { return 0.5; }
    size_t bucket_count() const noexcept { return d ? d->numBuckets : 0; }
    static size_t max_bucket_count() noexcept { return QHashPrivate::GrowthPolicy::maxNumBuckets(); }

    inline bool empty() const noexcept { return isEmpty(); }

    inline iterator replace(const Key &key, const T &value)
    {
        return emplaceReplace(key, value);
    }

    template <typename ...Args>
    iterator emplaceReplace(const Key &key, Args &&... args)
    {
        return emplaceReplace(Key(key), std::forward<Args>(args)...);
    }

    template <typename ...Args>
    iterator emplaceReplace(Key &&key, Args &&... args)
    {
        detach();
        auto result = d->findOrInsert(key);
        if (!result.initialized) {
            ++m_size;
            Node::createInPlace(result.it.node(), std::move(key), std::forward<Args>(args)...);
        }
        else {
            result.it.node()->emplaceValue(std::forward<Args>(args)...);
        }
        return iterator(result.it);
    }

    inline QMultiHash &operator+=(const QMultiHash &other) {
        this->unite(other);
        return *this;
    }
    inline QMultiHash operator+(const QMultiHash &other) const {
        QMultiHash result = *this;
        result += other;
        return result;
    }

    bool contains(const Key &key, const T &value) const noexcept {
        if (isEmpty()) {
            return false;
        }
        auto n = d->findNode(key);
        if (n == nullptr) {
            return false;
        }
        return n->value->contains(value);
    }

    qsizetype remove(const Key &key, const T &value) {
        if (isEmpty()) {
            return 0;
        }
        detach();

        auto it = d->find(key);
        if (it.isUnused()) {
            return 0;
        }
        qsizetype n = 0;
        Chain **e = &it.node()->value;
        while (*e) {
            Chain *entry = *e;
            if (entry->value == value) {
                *e = entry->next;
                delete entry;
                ++n;
            }
            else {
                e = &entry->next;
            }
        }

        if (!it.node()->value) {
            d->erase(it);
        }
        m_size -= n;
        Q_ASSERT(m_size >= 0);
        return n;
    }

    qsizetype count(const Key &key) const noexcept  {
        if (!d) {
            return 0;
        }
        auto it = d->find(key);
        if (it.isUnused()) {
            return 0;
        }
        qsizetype n = 0;
        Chain *e = it.node()->value;
        while (e) {
            ++n;
            e = e->next;
        }
        return n;
    }

    qsizetype count(const Key &key, const T &value) const noexcept {
        if (!d) {
            return 0;
        }
        auto it = d->find(key);
        if (it.isUnused()) {
            return 0;
        }
        qsizetype n = 0;
        Chain *e = it.node()->value;
        while (e) {
            if (e->value == value) {
                ++n;
            }
             e = e->next;
        }
        return n;
    }

    iterator find(const Key &key, const T &value) {
        if (isEmpty()) {
            return end();
        }
        detach();
        auto it = constFind(key, value);
        return iterator(it.i, it.e);
    }

    const_iterator find(const Key &key, const T &value) const noexcept  {
        return constFind(key, value);
    }

    const_iterator constFind(const Key &key, const T &value) const noexcept {
        const_iterator i(constFind(key));
        const_iterator end(constEnd());

        while (i != end && i.key() == key) {
            if (i.value() == value) {
                return i;
            }
            ++i;
        }
        return end;
    }

    QMultiHash &unite(const QMultiHash &other) {
        if (isEmpty()) {
            *this = other;
        }
        else if (other.isEmpty()) {

        }
        else {
            QMultiHash copy(other);  //zhaoyujie TODO 为啥要copy，直接cbegin cend不行吗？
            detach();
            for (auto it = copy.cbegin(); it != copy.cend(); ++it) {
                insert(it.key(), it.value());
            }
        }
        return *this;
    }

    QMultiHash &unite(const QHash<Key, T> &other) {
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
            insert(it.key(), it.value());
        }
        return *this;
    }

    QMultiHash &unite(QHash<Key, T > &&other) {
        if (!other.isDetached()) {
            unite(other);
            return *this;
        }
        //zhaoyujie TODO 这里效率可以优化，判断下当前的d是否为nullptr或者empty，可以直接把other.d swap过来
        auto it = other.d->begin();
        for (const auto end = other.d->end(); it != end; ++it) {
            //takeValue走了移动语义
            emplace(std::move(it.node()->key), std::move(it.node()->takeValue()));
        }
        other.clear();
        return *this;
    }

    QPair<iterator, iterator> equal_range(const Key &key) {
        detach();
        auto pair = qAsConst(*this).equal_range(key);
        return qMakePair(iterator(pair.first.i), iterator(pair.second.i));
    }

    QPair<const_iterator, const_iterator> equal_range(const Key &key) const noexcept {
        if (!d) {
            return qMakePair(end(), end());
        }
        auto it = d->find(key);
        if (it.isUnused()) {
            return qMakePair(end(), end());
        }
        //zhaoyujie TODO 这里只返回一个？
        auto end = it;
        ++end;
        return qMakePair(const_iterator(it), const_iterator(end));
    }

private:
    void detach_helper() {
        if (!d) {
            d = new Data;
            return;
        }
        Data *dd = new Data(*d);   //zhaoyujie TODO 这里如果ref为1，其实不需要拷贝
        if (!d->ref.deref()) {
            delete d;
        }
        d = dd;
    }
};

Q_DECLARE_ASSOCIATIVE_FORWARD_ITERATOR(Hash)
Q_DECLARE_MUTABLE_ASSOCIATIVE_FORWARD_ITERATOR(Hash)
Q_DECLARE_ASSOCIATIVE_FORWARD_ITERATOR(MultiHash)
Q_DECLARE_MUTABLE_ASSOCIATIVE_FORWARD_ITERATOR(MultiHash)

template <class Key, class T>
size_t qHash(const QHash<Key, T> &key, size_t seed = 0) noexcept
{
    size_t hash = 0;
    for (auto it = key.begin(), end = key.end(); it != end; ++it) {
        QtPrivate::QHashCombine combine;
        size_t h = combine(seed, it.key());
        hash += combine(h, it.value());
    }
    return hash;
}

template <class Key, class T>
inline size_t qHash(const QMultiHash<Key, T> &key, size_t seed = 0) noexcept
{

    size_t hash = 0;
    for (auto it = key.begin(), end = key.end(); it != end; ++it) {
        QtPrivate::QHashCombine combine;
        size_t h = combine(seed, it.key());
        hash += combine(h, it.value());
    }
    return hash;
}

template <typename Key, typename T, typename Predicate>
qsizetype erase_if(QHash<Key, T> &hash, Predicate pred)
{
    return QtPrivate::associative_erase_if(hash, pred);
}

template <typename Key, typename T, typename Predicate>
qsizetype erase_if(QMultiHash<Key, T> &hash, Predicate pred)
{
    return QtPrivate::associative_erase_if(hash, pred);
}

QT_END_NAMESPACE

#endif //QHASH_H