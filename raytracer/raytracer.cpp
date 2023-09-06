#include "image.h"
#include "kdtree.h"
#include "ray.h"
#include "raytracer.h"
#include "scene_types.h"
#include "Mesh/MeshTriangle.h"
#include <stdio.h>

#include <glm/gtc/epsilon.hpp>

using namespace glm;

/// acne_eps is a small constant used to prevent acne when computing
/// intersection
//  or boucing (add this amount to the position before casting a new ray !
const float acne_eps = 1e-4;

bool intersectPlane(Ray *ray, Intersection *intersection, Object *obj)
{
    if (dot(ray->dir, obj->geom.plane.normal) != 0)
    {
        float t = -(dot(ray->orig, obj->geom.plane.normal) + obj->geom.plane.dist) / (dot(ray->dir, obj->geom.plane.normal));
        if (t >= ray->tmin && t <= ray->tmax)
        {
            intersection->normal = obj->geom.plane.normal;
            intersection->position = ray->orig + t * ray->dir;
            intersection->mat = &(obj->mat);
            ray->tmax = t;
            return true;
        }
    }
    return false;
}

bool intersectSphere(Ray *ray, Intersection *intersection, Object *obj)
{
    float t;
    float delta = (2 * dot(ray->dir, ray->orig - obj->geom.sphere.center) * 2 * dot(ray->dir, ray->orig - obj->geom.sphere.center)) - 4 * (dot(ray->orig - obj->geom.sphere.center, ray->orig - obj->geom.sphere.center) - obj->geom.sphere.radius * obj->geom.sphere.radius);

    if (delta > 0) //deux solutions
    {
        float t1 = (-2 * dot(ray->dir, ray->orig - obj->geom.sphere.center) + sqrt(delta)) / 2;
        float t2 = (-2 * dot(ray->dir, ray->orig - obj->geom.sphere.center) - sqrt(delta)) / 2;

        if (t1 >= ray->tmin && t1 <= ray->tmax && t2 >= ray->tmin && t2 <= ray->tmax)
        {
            t = glm::min(t1, t2);
        }
        else if (t1 >= ray->tmin && t1 <= ray->tmax)
        {
            t = t1;
        }
        else if (t2 >= ray->tmin && t2 <= ray->tmax)
        {
            t = t2;
        }
        else
        {
            return false;
        }
    }
    else if (delta == 0) //une solutions
    {
        t = (-2 * dot(ray->dir, ray->orig - obj->geom.sphere.center)) / 2;
        if (!(t >= ray->tmin && t <= ray->tmax))
        {
            return false;
        }
    }
    else //pas de solution
    {
        return false;
    }

    //calcul de la normale du point
    point3 intersec = ray->orig + t * ray->dir;
    vec3 normal = intersec - obj->geom.sphere.center;
    normal = normalize(normal);

    intersection->normal = normal;
    intersection->position = ray->orig + t * ray->dir;
    intersection->mat = &(obj->mat);
    ray->tmax = t;
    return true;
}

bool intersectTriangle(Ray *ray, Intersection *intersection, MeshTriangle *triangle)
{
    vec3 u = triangle->getU();
    vec3 v = triangle->getV();
    vec3 n = cross(u, v);
    if (dot(n, ray->dir) == 0) //le rayon est parallele au triangle
    {
        return false;
    }
    vec3 A = triangle->getA();
    float D = dot(-n, ray->dir);

    vec3 w = vec3(ray->orig.x - A.x, ray->orig.y - A.y, ray->orig.z - A.z);

    float a = dot(-cross(w, v), ray->dir) / D;
    float b = dot(-cross(u, w), ray->dir) / D;
    float t = dot(n, w) / D;

    if (((a >= 0 && b >= 0) && (a + b) <= 1) && t > ray->tmin && t < ray->tmax)
    {
        ray->tmax = t;
        intersection->position = ray->orig + ray->dir * t;
        intersection->normal = triangle->getNormal();
        intersection->mat = triangle->getMaterial();

        return true;
    }
    return false;
}

bool intersectScene(Scene *scene, Ray *ray, Intersection *intersection)
{
    bool hasIntersected = false;
    size_t objectCount = scene->objects.size();

    for (int i = 0; i < objectCount; i++)
    {
        if (scene->objects[i]->geom.type == SPHERE)
        {
            if (intersectSphere(ray, intersection, scene->objects[i]))
            {
                hasIntersected = true;
            }
        }
        else
        {
            if (intersectPlane(ray, intersection, scene->objects[i]))
            {
                hasIntersected = true;
            }
        }
    }

    size_t meshCount = scene->meshes.size();

    for (int i = 0; i < meshCount; i++)
    {
        vector<MeshTriangle *> triangles = scene->meshes[i].getTriangles();
        size_t triangleCount = triangles.size();

        for (int j = 0; j < triangleCount; j++)
        {
            if (intersectTriangle(ray, intersection, triangles[j]))
            {
                hasIntersected = true;
            }
        }
    }

    return hasIntersected;
}

/* ---------------------------------------------------------------------------
 */
/*
 *	The following functions are coded from Cook-Torrance bsdf model
 *description and are suitable only
 *  for rough dielectrics material (RDM. Code has been validated with Mitsuba
 *renderer)
 */

// Shadowing and masking function. Linked with the NDF. Here, Smith function,
// suitable for Beckmann NDF
float RDM_chiplus(float c) { return (c > 0.f) ? 1.f : 0.f; }

/** Normal Distribution Function : Beckmann
 * NdotH : Norm . Half
 */
float RDM_Beckmann(float NdotH, float alpha)
{
    if (NdotH > 0)
    {
        return exp(-((1 - NdotH * NdotH) / (NdotH * NdotH)) / (alpha * alpha)) / (pi<float>() * alpha * alpha * pow(NdotH, 4));
    }

    return 0.f;
}

// Fresnel term computation. Implantation of the exact computation. we can use
// the Schlick approximation
// LdotH : Light . Half
float RDM_Fresnel(float LdotH, float extIOR, float intIOR)
{
    float sin2Ot = pow((extIOR / intIOR), 2) * (1 - LdotH * LdotH);
    if (sin2Ot > 1)
    {
        return 1;
    }

    float cosOt = sqrt(1 - sin2Ot);

    float rs = pow(extIOR * LdotH - intIOR * cosOt, 2) / pow(extIOR * LdotH + intIOR * cosOt, 2);
    float rp = pow(extIOR * cosOt - intIOR * LdotH, 2) / pow(extIOR * cosOt + intIOR * LdotH, 2);

    return 0.5 * (rs + rp);
}

// DdotH : Dir . Half
// HdotN : Dir . Norm
float RDM_G1(float DdotH, float XdotN, float alpha)
{
    float tan0x = sqrt(1 - XdotN * XdotN) / XdotN;
    float b = 1 / (alpha * tan0x);
    float k = DdotH / XdotN;

    if (b < 1.6)
    {
        if (k > 0)
        {
            return (3.535 * b + 2.18 * b * b) / (1 + 2.276 * b + 2.577 * b * b);
        }
        return 0;
    }
    if (k > 0)
    {
        return 1;
    }

    return 0;
}

// LdotH : Light . Half
// LdotN : Light . Norm
// VdotH : View . Half
// VdotN : View . Norm
float RDM_Smith(float LdotH, float LdotN, float VdotH, float VdotN, float alpha)
{
    float g11 = RDM_G1(LdotH, LdotN, alpha);
    float g12 = RDM_G1(VdotH, VdotN, alpha);

    return g11 * g12;
}

// Specular term of the Cook-torrance bsdf
// LdotH : Light . Half
// NdotH : Norm . Half
// VdotH : View . Half
// LdotN : Light . Norm
// VdotN : View . Norm
color3 RDM_bsdf_s(float LdotH, float NdotH, float VdotH, float LdotN, float VdotN, Material *m)
{
    float D = RDM_Beckmann(NdotH, m->roughness);
    float F = RDM_Fresnel(LdotH, 1, m->IOR);
    float G = RDM_Smith(LdotH, LdotN, VdotH, VdotN, m->roughness);

    return color3(m->specularColor * (D * F * G) / (4 * LdotN * VdotN));
}

// diffuse term of the cook torrance bsdf
color3 RDM_bsdf_d(Material *m)
{
    return color3((m->diffuseColor) / pi<float>());
}

// The full evaluation of bsdf(wi, wo) * cos (thetai)
// LdotH : Light . Half
// NdotH : Norm . Half
// VdotH : View . Half
// LdotN : Light . Norm
// VdtoN : View . Norm
// compute bsdf * cos(Oi)
color3 RDM_bsdf(float LdotH, float NdotH, float VdotH, float LdotN, float VdotN, Material *m)
{
    return color3(RDM_bsdf_d(m) + RDM_bsdf_s(LdotH, NdotH, VdotH, LdotN, VdotN, m));
}

color3 shade(vec3 n, vec3 v, vec3 l, color3 lc, Material *mat)
{
    vec3 h = normalize(v + l);

    if (dot(l, n) < 0)
    {
        return color3(0.f);
    }

    return RDM_bsdf(dot(l, h), dot(n, h), dot(v, h), dot(l, n), dot(v, n), mat) * dot(l, n) * lc;
}

//! if tree is not null, use intersectKdTree to compute the intersection instead
//! of intersect scene

color3 trace_ray(Scene *scene, Ray *ray, KdTree *tree)
{
    //limite de reflexion
    if (ray->depth == 3)
    {
        return color3(0.f);
    }

    color3 ret = scene->skyColor;
    Intersection intersection, sIntersection;
    Ray sRay, reflectRay;

    if (intersectScene(scene, ray, &intersection))
    {
        ret = color3(0.f);
        //pour chaque lumiere dans la scene
        for (int i = 0; i < scene->lights.size(); i++)
        {
            //on vérifie que l'objet est dans la lumiere
            rayInit(&sRay, intersection.position + acne_eps * normalize(scene->lights[i]->position - intersection.position), normalize(scene->lights[i]->position - intersection.position), 0, distance(scene->lights[i]->position, intersection.position));
            if (!intersectScene(scene, &sRay, &sIntersection))
            {
                //on additionne la contribution de la lumiere
                ret += shade(intersection.normal, -ray->dir, normalize(scene->lights[i]->position - intersection.position), scene->lights[i]->color, intersection.mat);
            }
        }
        vec3 reflectVec = reflect<vec3>(ray->dir, intersection.normal);
        rayInit(&reflectRay, intersection.position + acne_eps * normalize(reflectVec), normalize(reflectVec), 0, 100000, ray->depth + 1);
        ret += trace_ray(scene, &reflectRay, tree) * intersection.mat->specularColor * RDM_Fresnel(dot(reflectVec, intersection.normal), 1, intersection.mat->IOR);
    }

    return ret;
}

void renderImage(Image *img, Scene *scene)
{

    //! This function is already operational, you might modify it for antialiasing
    //! and kdtree initializaion
    float aspect = 1.f / scene->cam.aspect;

    KdTree *tree = NULL;

    //! \todo initialize KdTree

    float delta_y = 1.f / (img->height * 0.5f);   //! one pixel size
    vec3 dy = delta_y * aspect * scene->cam.ydir; //! one pixel step
    vec3 ray_delta_y = (0.5f - img->height * 0.5f) / (img->height * 0.5f) *
                       aspect * scene->cam.ydir;

    float delta_x = 1.f / (img->width * 0.5f);
    vec3 dx = delta_x * scene->cam.xdir;
    vec3 ray_delta_x =
        (0.5f - img->width * 0.5f) / (img->width * 0.5f) * scene->cam.xdir;

    for (size_t j = 0; j < img->height; j++)
    {
        if (j != 0)
            std::printf("\033[A\r");
        float progress = (float)j / img->height * 100.f;
        std::printf("progress\t[");
        int cpt = 0;
        for (cpt = 0; cpt < progress; cpt += 5)
            std::printf(".");
        for (; cpt < 100; cpt += 5)
            std::printf(" ");
        std::printf("]\n");
#pragma omp parallel for
        for (size_t i = 0; i < img->width; i++)
        {
            color3 *ptr = getPixelPtr(img, i, j);
            vec3 ray_dir = scene->cam.center + ray_delta_x + ray_delta_y +
                           float(i) * dx + float(j) * dy;

            Ray rx;
            rayInit(&rx, scene->cam.position, normalize(ray_dir));
            *ptr = trace_ray(scene, &rx, tree);
        }
    }
}