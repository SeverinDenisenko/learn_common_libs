// Copyright 2024 Severin Denisenko

#pragma once

#include <memory>

#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/unordered_set_hook.hpp>

#include <boost/container/vector.hpp>

template <typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class lru_map {
public:
    using link_mode = boost::intrusive::link_mode<
#ifdef NDEBUG
    boost::inintrusive::normal_link
#else
    boost::intrusive::safe_link
#endif
    >;

    using lru_list_hook = boost::intrusive::list_base_hook<link_mode>;
    using lru_hash_set_hook = boost::intrusive::unordered_set_base_hook<link_mode>;

    class lru_node final : public lru_list_hook, public lru_hash_set_hook {
    public:
        explicit lru_node(Key key, Value&& value)
            : key_(std::move(key))
            , value_(std::move(value))
        {
        }

        const Key& get_key() const noexcept { return key_; }

        const Value& get_value() const noexcept { return value_; }

        void set_value(Value&& value) { value_ = std::move(value); }

        void set_key(Key key) { key_ = std::move(key); }

    private:
        Key key_;
        Value value_;
    };

    struct lru_node_hash : Hash {
        auto operator()(const Key& a) const { return Hash::operator()(a); }

        auto operator()(const lru_node& a) const
        {
            return Hash::operator()(a.get_key());
        }
    };

    struct lru_node_equal : Equal {
        auto operator()(const lru_node& a, const lru_node& b) const
        {
            return Equal::operator()(a.get_key(), b.get_key());
        }

        auto operator()(const Key& a, const lru_node& b) const
        {
            return Equal::operator()(a, b.get_key());
        }

        auto operator()(const lru_node& a, const Key& b) const
        {
            return Equal::operator()(a.get_key(), b);
        }

        auto operator()(const Key& a, const Key& b) const
        {
            return Equal::operator()(a, b);
        }
    };

    using map = boost::intrusive::unordered_set<lru_node,
    boost::intrusive::constant_time_size<true>,
    boost::intrusive::hash<lru_node_hash>,
    boost::intrusive::equal<lru_node_equal>>;

    using bucket_traits = map::bucket_traits;
    using bucket        = map::bucket_type;
    using buckets       = boost::container::vector<bucket>;

    using list =
    boost::intrusive::list<lru_node, boost::intrusive::constant_time_size<false>>;

    std::size_t max_size_;
    buckets buckets_;
    bucket_traits bucket_traits_;
    map map_;
    list list_;

    std::unique_ptr<lru_node> extract_node(typename list::iterator it) noexcept
    {
        std::unique_ptr<lru_node> ret(&*it);
        map_.erase(map_.iterator_to(*it));
        list_.erase(it);
        return ret;
    }

    void insert_node(std::unique_ptr<lru_node> node) noexcept
    {
        if(!node) {
            return;
        }

        map_.insert(*node);
        list_.insert(list_.end(), *node);
        [[maybe_unused]] auto ignore = node.release();
    }

public:
    lru_map(size_t max_size)
        : max_size_(max_size)
        , buckets_(max_size_)
        , bucket_traits_(buckets_.data(), max_size_)
        , map_(bucket_traits_)
        , list_()
    {
    }

    bool contains(const Key& key)
    {
        auto it = map_.find(key, map_.hash_function(), map_.key_eq());
        if(it == map_.end()) {
            return false;
        }

        list_.splice(list_.end(), list_, list_.iterator_to(*it));
        return true;
    }

    bool put(const Key& key, Value&& value)
    {
        auto it = map_.find(key, map_.hash_function(), map_.key_eq());
        if(it != map_.end()) {
            list_.splice(list_.end(), list_, list_.iterator_to(*it));
            return false;
        }
        if(map_.size() == max_size_) {
            auto node = extract_node(list_.begin());
            node->set_key(key);
            node->set_value(std::move(value));
            insert_node(std::move(node));
        }
        else {
            auto node = std::make_unique<lru_node>(key, std::move(value));
            insert_node(std::move(node));
        }
        return true;
    }

    list::const_iterator begin() const { return list_.begin(); }

    list::const_iterator end() const { return list_.end(); }
};
