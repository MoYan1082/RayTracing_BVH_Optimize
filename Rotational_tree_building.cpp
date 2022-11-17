// Rotational tree building
shared_ptr<bvh_node> bvh_node::bvh_node_optimize1 (
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1, int dep
) {
    auto res = make_shared<bvh_node>();
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = dep % 3; // choose an axis by depth
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                : box_z_compare;

    size_t object_span = end - start;
    if (object_span == 1) {
        res->left = res->right = objects[start];
    } else if (object_span == 2) {
        if (comparator(objects[start], objects[start+1])) {
            res->left = objects[start];
            res->right = objects[start+1];
        } else {
            res->left = objects[start+1];
            res->right = objects[start];
        }
    } else {
        auto mid = start + object_span/2;
        std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end, comparator);
        res->left  = bvh_node_optimize1(objects, start, mid, time0, time1, dep + 1);
        res->right = bvh_node_optimize1(objects, mid, end, time0, time1, dep + 1);
    }
    aabb box_left, box_right;

    if (!res->left->bounding_box (time0, time1, box_left)
    || !res->right->bounding_box(time0, time1, box_right)
    )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    res->box = surrounding_box(box_left, box_right);
    return res;
}