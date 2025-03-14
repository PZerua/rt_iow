#pragma once

#include "hittables/hittable_list.h"
#include "hittables/bvh_node.h"
#include "hittables/sphere.h"
#include "hittables/quad.h"

#include "graphics/materials/lambertian_material.h"
#include "graphics/materials/metallic_material.h"
#include "graphics/materials/dielectric_material.h"
#include "graphics/materials/diffuse_light.h"

#include "graphics/rt_camera.h"

namespace scenes {

    void three_spheres(HittableList& world, sTracingCamera& tracing_camera)
    {
        RtMaterial* material_ground = new LambertianMaterial({ 0.8, 0.8, 0.0 });
        RtMaterial* material_center = new LambertianMaterial({ 0.1, 0.2, 0.5 });
        RtMaterial* material_left = new DielectricMaterial(1.5);
        RtMaterial* material_bubble = new DielectricMaterial(1.0 / 1.5);
        RtMaterial* material_right = new MetallicMaterial({ 0.8, 0.6, 0.2 }, 1.0);

        //world.add(new Sphere(glm::dvec3(0.0, -100.5, -1.0), 100.0, material_ground));

        Sphere* sphere = new Sphere(glm::dvec3(0.0, 0.0, -1.2), 0.5, material_center);
        sphere->set_position(glm::vec3(0, 0.5, 0));

        world.add(sphere);
        //world.add(new Sphere(glm::dvec3(-1.0, 0.0, -1.0), 0.5, material_left));
        //world.add(new Sphere(glm::dvec3(-1.0, 0.0, -1.0), 0.4, material_bubble));
        //world.add(new Sphere(glm::dvec3(1.0, 0.0, -1.0), 0.5, material_right));

        tracing_camera.lookfrom = glm::dvec3(0.0);
        tracing_camera.lookat = glm::dvec3(0.0, 0.0, -1.0);
        tracing_camera.vup = glm::dvec3(0.0, 1.0, 0.0);

        tracing_camera.samples_per_pixel = 1;

        tracing_camera.vfov = 90;

        tracing_camera.background = { 0.7, 0.8, 1.0 };
    }

    void bouncing_spheres(HittableList& world, sTracingCamera& tracing_camera)
    {

        RtTexture* checker = new CheckerTexture(0.32, glm::dvec3(.2, .3, .1), glm::dvec3(.9, .9, .9));
        world.add(new Sphere(glm::dvec3(0, -1000, 0), 1000, new LambertianMaterial(checker)));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                double choose_mat = random_d();
                glm::dvec3 center(a + 0.9 * random_d(), 0.2, b + 0.9 * random_d());

                if (glm::length((center - glm::dvec3(4, 0.2, 0))) > 0.9) {
                    RtMaterial* sphere_material;

                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = random_color() * random_color();
                        sphere_material = new LambertianMaterial(albedo);
                        auto center2 = center + glm::dvec3(0, random_d(0, .5), 0);
                        world.add(new Sphere(center, center2, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = random_color(0.5, 1);
                        auto fuzz = random_d(0, 0.5);
                        sphere_material = new MetallicMaterial(albedo, fuzz);
                        world.add(new Sphere(center, 0.2, sphere_material));
                    }
                    else {
                        // glass
                        sphere_material = new DielectricMaterial(1.5);
                        world.add(new Sphere(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = new DielectricMaterial(1.5);
        world.add(new Sphere({ 0, 1, 0 }, 1.0, material1));

        auto material2 = new LambertianMaterial({ 0.4, 0.2, 0.1 });
        world.add(new Sphere({ -4, 1, 0 }, 1.0, material2));

        auto material3 = new MetallicMaterial({ 0.7, 0.6, 0.5 }, 0.0);
        world.add(new Sphere({ 4, 1, 0 }, 1.0, material3));

        tracing_camera.samples_per_pixel = 10;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 20;
        tracing_camera.lookfrom = glm::dvec3(13, 2, 3);
        tracing_camera.lookat = glm::dvec3(0, 0, 0);
        tracing_camera.vup = glm::dvec3(0.0, 1.0, 0.0);

        tracing_camera.defocus_angle = 0.6;
        tracing_camera.focus_dist = 10.0;

        tracing_camera.background = { 0.7, 0.8, 1.0 };
    }

    void checkered_spheres(HittableList& world, sTracingCamera& tracing_camera)
    {
        RtTexture* checker = new CheckerTexture(0.32, glm::dvec3(.2, .3, .1), glm::dvec3(.9, .9, .9));

        world.add(new Sphere(glm::dvec3(0, -10, 0), 10, new LambertianMaterial(checker)));
        world.add(new Sphere(glm::dvec3(0, 10, 0), 10, new LambertianMaterial(checker)));

        tracing_camera.samples_per_pixel = 1;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 20;
        tracing_camera.lookfrom = glm::dvec3(13, 2, 3);
        tracing_camera.lookat = glm::dvec3(0, 0, 0);
        tracing_camera.vup = glm::dvec3(0, 1, 0);

        tracing_camera.defocus_angle = 0;

        tracing_camera.background = { 0.7, 0.8, 1.0 };
    }

    void earth(HittableList& world, sTracingCamera& tracing_camera)
    {
        auto earth_texture = new ImageTexture("data/textures/earthmap.jpg");
        auto earth_surface = new LambertianMaterial(earth_texture);
        world.add(new Sphere(glm::dvec3(0, 0, 0), 2, earth_surface));

        tracing_camera.samples_per_pixel = 100;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 20;
        tracing_camera.lookfrom = glm::dvec3(0, 0, 12);
        tracing_camera.lookat = glm::dvec3(0, 0, 0);
        tracing_camera.vup = glm::dvec3(0, 1, 0);

        tracing_camera.defocus_angle = 0;

        tracing_camera.background = { 0.7, 0.8, 1.0 };
    }

    void quads(HittableList& world, sTracingCamera& tracing_camera)
    {
        // Materials
        auto left_red = new LambertianMaterial(glm::dvec3(1.0, 0.2, 0.2));
        auto back_green = new LambertianMaterial(glm::dvec3(0.2, 1.0, 0.2));
        auto right_blue = new LambertianMaterial(glm::dvec3(0.2, 0.2, 1.0));
        auto upper_orange = new LambertianMaterial(glm::dvec3(1.0, 0.5, 0.0));
        auto lower_teal = new LambertianMaterial(glm::dvec3(0.2, 0.8, 0.8));

        // Quads
        world.add(new Quad(glm::dvec3(-3, -2, 5), glm::dvec3(0, 0, -4), glm::dvec3(0, 4, 0), left_red));
        world.add(new Quad(glm::dvec3(-2, -2, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 4, 0), back_green));
        world.add(new Quad(glm::dvec3(3, -2, 1), glm::dvec3(0, 0, 4), glm::dvec3(0, 4, 0), right_blue));
        world.add(new Quad(glm::dvec3(-2, 3, 1), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, 4), upper_orange));
        world.add(new Quad(glm::dvec3(-2, -3, 5), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), lower_teal));

        tracing_camera.samples_per_pixel = 100;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 80;
        tracing_camera.lookfrom = glm::dvec3(0, 0, 9);
        tracing_camera.lookat = glm::dvec3(0, 0, 0);
        tracing_camera.vup = glm::dvec3(0, 1, 0);

        tracing_camera.defocus_angle = 0;

        tracing_camera.background = { 0.7, 0.8, 1.0 };
    }

    void simple_light(HittableList& world, sTracingCamera& tracing_camera)
    {
        auto lambertian = new LambertianMaterial(glm::dvec3(1.0, 0.2, 0.2));
        world.add(new Sphere(glm::dvec3(0, -1000, 0), 1000, lambertian));
        world.add(new Sphere(glm::dvec3(0, 2, 0), 2, lambertian));

        auto difflight = new DiffuseLight(glm::dvec3(4, 4, 4));
        world.add(new Quad(glm::dvec3(3, 1, -2), glm::dvec3(2, 0, 0), glm::dvec3(0, 2, 0), difflight));

        tracing_camera.samples_per_pixel = 100;
        tracing_camera.max_depth = 50;

        tracing_camera.vfov = 20;
        tracing_camera.lookfrom = glm::dvec3(26, 3, 6);
        tracing_camera.lookat = glm::dvec3(0, 2, 0);
        tracing_camera.vup = glm::dvec3(0, 1, 0);

        tracing_camera.defocus_angle = 0;
    }

    void cornell_box(HittableList& world, sTracingCamera& tracing_camera)
    {
        RtMaterial* red = new LambertianMaterial(glm::dvec3(.65, .05, .05));
        RtMaterial* white = new LambertianMaterial(glm::dvec3(.73, .73, .73));
        RtMaterial* green = new LambertianMaterial(glm::dvec3(.12, .45, .15));
        RtMaterial* light = new DiffuseLight(glm::dvec3(15, 15, 15));

        world.add(new Quad(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green));
        world.add(new Quad(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));
        world.add(new Quad(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), light));
        world.add(new Quad(glm::dvec3(0, 0, 0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), white));
        world.add(new Quad(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), white));
        world.add(new Quad(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), white));

        world.add(box(glm::dvec3(130, 0, 65), glm::dvec3(295, 165, 230), white));
        world.add(box(glm::dvec3(265, 0, 295), glm::dvec3(430, 330, 460), white));

        tracing_camera.samples_per_pixel = 2;
        tracing_camera.max_depth = 50;
        tracing_camera.background = glm::dvec3(0, 0, 0);

        tracing_camera.vfov = 40;
        tracing_camera.lookfrom = glm::dvec3(278, 278, -800);
        tracing_camera.lookat = glm::dvec3(278, 278, 0);
        tracing_camera.vup = glm::dvec3(0, 1, 0);

        tracing_camera.defocus_angle = 0;
    }
}
