## RayTracing_BVH_Optimize

### :key:思路来源

我在学习[Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)的时候，在Bounding Volume Hierarchies中发现作者建树时采用的策略是：

1. randomly choose an axis
2. sort the primitives (`using std::sort`)
3. put half in each subtree

我感觉这个**随机算法**不太优秀，于是我就想到了之前学KD-Tree时学过的一些建树策略：**轮转建树**和**方差建树**。因此我对原本的代码做了一些修改。

除此之外，我还优化掉了 `std::sort` 函数，因为我们只需要找到**中间元素的位置**（用来划分子树），而不需要对全部元素排序。其中 `std::sort` 的复杂度为 $O(n\log n)$。我采用了类似**快排**的思想，用**分治**来进行这一步的操作，代码中对应的就是 `std::nth_element()` 函数，且它的复杂度为 $O(n)$ 。

### :hammer_and_wrench:如何使用

1. 在 `[bvh.h]` 的 `bvh_node` 类中增加两个成员函数 ：

```cpp
// Rotational tree building
shared_ptr<bvh_node> bvh_node_optimize1 (
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1, int dep
);
// variance tree building
shared_ptr<bvh_node> bvh_node_optimize2 (
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1
);
```

​	其中具体的实现在文件 [Rotational_tree_building](./Rotational_tree_building.cpp) 和 [Variance_tree_building](./Variance_tree_building.cpp) 中。

2. 增加 `bvh_node` 的无参构造函数：

```cpp
bvh_node() : left(nullptr), right(nullptr) {}
```

3. 修改 `bvh_node` 的含参构造函数：

```cpp
bvh_node::bvh_node(
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1
) {
    auto res = bvh_node_optimize1(src_objects, start, end, time0, time1, 0);
    // auto res = bvh_node_optimize2(src_objects, start, end, time0, time1);
    left  = res->left;
    right = res->right;
    box = res->box;
}
```

### :timer_clock:优化效果

OS: Ubuntu 22.04.1 LTS on Windows 10 x86_64；

随机化种子：srand(1024)；

以 `hittable_list random_scene();` 为基准程序进行比较，结果如下：

| 建树方式       | 运行时间(s) |
| -------------- | ----------- |
| 随机建树(base) | 312         |
| 轮转建树       | 219         |
| 方差建树       | 209         |



参考：[Oi-Wiki-Kd-Tree](https://oi-wiki.org/ds/kdt/).