// variance tree building
shared_ptr<bvh_node> bvh_node::bvh_node_optimize2 (
    const std::vector<shared_ptr<hittable>>& src_objects,
    size_t start, size_t end, double time0, double time1
) {
    auto res = make_shared<bvh_node>();
    auto objects = src_objects; // Create a modifiable array of the source scene objects
    size_t object_span = end - start;
    auto mid = start + object_span/2;

    double avx = 0, avy = 0, avz = 0; // average
    double vax = 0, vay = 0, vaz = 0; // variance
    for (int i = start; i < end; i++) {
        aabb box_i;
        if (!objects[i]->bounding_box(time0, time1, box_i))
            std::cerr << "No bounding box in bvh_node constructor.\n";
        double tmp_x = (box_i.min()[0] + box_i.max()[0]) / 2;
        double tmp_y = (box_i.min()[1] + box_i.max()[1]) / 2;
        double tmp_z = (box_i.min()[2] + box_i.max()[2]) / 2;
        avx += tmp_x;
        avy += tmp_y;
        avz += tmp_z;
    }
    avx /= 1.0 * (end - start);
    avy /= 1.0 * (end - start);
    avz /= 1.0 * (end - start);

    for (int i = start; i < end; i++) {
        aabb box_i;
        if (!objects[i]->bounding_box(time0, time1, box_i))
            std::cerr << "No bounding box in bvh_node constructor.\n";
        double tmp_x = (box_i.min()[0] + box_i.max()[0]) / 2;
        double tmp_y = (box_i.min()[1] + box_i.max()[1]) / 2;
        double tmp_z = (box_i.min()[2] + box_i.max()[2]) / 2;
        vax += (tmp_x - avx) * (tmp_x - avx);
        vay += (tmp_y - avy) * (tmp_y - avy);
        vaz += (tmp_z - avz) * (tmp_z - avz);
    }
    vax /= 1.0 * (end - start);
    vay /= 1.0 * (end - start);
    vaz /= 1.0 * (end - start);

    int axis = 0; // choose an axis by variance
    if (vax >= vay && vax >= vaz) 
        axis = 0;
    else if (vay >= vax && vay >= vaz) 
        axis = 1;
    else
        axis = 2;

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                : box_z_compare;
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
        std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end, comparator);
        res->left  = bvh_node_optimize2(objects, start, mid, time0, time1);
        res->right = bvh_node_optimize2(objects, mid, end, time0, time1);
    }
    aabb box_left, box_right;

    if (!res->left->bounding_box (time0, time1, box_left)
    || !res->right->bounding_box(time0, time1, box_right)
    )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    res->box = surrounding_box(box_left, box_right);
    return res;
}