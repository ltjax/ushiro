#pragma once


#include <utility>
#include <vector>
#include <unordered_set>

namespace ushiro
{
template <typename KeyType, typename ItemType>
class diffable_list
{
public:
  template <typename T, typename Hasher>
  static bool contains(std::unordered_set<T, Hasher> const& set, T rhs)
  {
    return set.find(rhs) != set.end();
  }

  template <typename T, typename ExtractId, typename InsertItem, typename RemoveItem>
  void update(T const& container, ExtractId&& extract_id, InsertItem&& insert_item, RemoveItem&& remove_item)
  {
    // Build a set of new ids
    std::unordered_set<KeyType> newIds;
    for (auto const& each : container)
      newIds.insert(extract_id(each));

    // Delete those that are no longer necessary
    auto target = children_.begin();
    for (auto i = target; i != children_.end(); ++i)
    {
      auto const& each = *i;

      if (contains(newIds, each.first))
      {
        *target++ = *i;
      }
      else
      {
        auto removal_index = static_cast<int>(target - children_.begin());
        remove_item(std::move(each.second), removal_index);
      }
    }
    children_.erase(target, children_.end());

    // Add new widgets
    int target_index = 0;
    auto N = static_cast<int>(children_.size());
    for (auto const& each : container)
    {
      if (target_index < N && extract_id(each) == children_[target_index].first)
      {
        target_index++;
        continue;
      }
      ItemType widget = insert_item(each, target_index);
      children_.insert(children_.begin() + target_index, std::make_pair(extract_id(each), std::move(widget)));
      target_index++;
    }
  }

private:
  std::vector<std::pair<KeyType, ItemType>> children_;
};

}


