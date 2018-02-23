/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Tuesday, October 17, 2017 - 10:24:56
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <dh2320lab3/sphere.h>
#include <dh2320lab3/util.h>

namespace inviwo {

Sphere::Sphere(const vec3& center, const double& radius) {
    center_ = center;
    radius_ = radius;
}

bool Sphere::closestIntersection(const Ray& ray, double maxLambda,
                                 RayIntersection& intersection) const {
    //Programming TASK 1: implement this method
    //Your code should compute the intersection between a ray and a sphere;

    //If you detect an intersection, the return type should look similar to this:
    //if(rayIntersectsSphere)
    //{
    //  intersection = RayIntersection(ray,shared_from_this(),lambda,ray.pointOnRay(lambda),uvw);
    //  return true;
    //}
    //
    // Hints :
    // As explained in Lecture on Raytracing
    // 1. Ray origin p_r : ray.getOrigin()
    //    Ray direction t_r : ray.getDirection()
    // 2. Solving the quadratic equation (ax^2 + bx + c = 0) for an intersection of a ray with a sphere
    //    results in two solutions if the discriminant is positive  
    // 3. Solving for lambda we have the values  lambda1 = (-b - sqrt(b^2 - 4ac)) / 2a and
    //    lambda2 = (-b + sqrt(b^2 - 4ac)) / 2a
    // 4. If lambda1 < 0, then the intersection lies inside the sphere. choose lambda2.
    // 5. lambda must be less than maxLambda, otherwise return false
    // 6. compute intersection point using ray.pointOnRay(lambda)

    return false;
}

bool Sphere::anyIntersection(const Ray& ray, double maxLambda) const {
    RayIntersection temp;
    return closestIntersection(ray, maxLambda, temp);
}

void Sphere::drawGeometry(std::shared_ptr<BasicMesh> mesh,
                          std::vector<BasicMesh::Vertex>& vertices) const {
    auto indexBuffer = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);

    int lat = 8;
    int lon = 10;

    for (int i = 0; i < lat - 1; i++) {
        float theta1 = float(i * M_PI) / (lat - 1);
        float theta2 = float((i + 1) * M_PI) / (lat - 1);

        for (int j = 0; j < lon - 1; j++) {
            float phi1 = float(j * 2 * M_PI) / (lon - 1);
            float phi2 = float((j + 1) * 2 * M_PI) / (lon - 1);

            vec3 v1 = vec3(radius_ * sin(theta1) * cos(phi1), radius_ * sin(theta1) * sin(phi1),
                           radius_ * cos(theta1)) + center_;
            vec3 v2 = vec3(radius_ * sin(theta2) * cos(phi1), radius_ * sin(theta2) * sin(phi1),
                           radius_ * cos(theta2)) + center_;
            vec3 v3 = vec3(radius_ * sin(theta2) * cos(phi2), radius_ * sin(theta2) * sin(phi2),
                           radius_ * cos(theta2)) + center_;

            Util::drawLineSegment(v1, v2, vec4(0.2, 0.2, 0.2, 1), indexBuffer, vertices);
            Util::drawLineSegment(v2, v3, vec4(0.2, 0.2, 0.2, 1), indexBuffer, vertices);
        }
    }
}

} // namespace
