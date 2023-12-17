#ifndef MATERIALH
#define MATERIALH

struct hit_record;

#include "ray.h"
#include "hitable.h"


// Изменение отражательной способности в зависимости от угла под которым мы смотрим
// на отражающую поверхность. Основано на полиномиальной аппрокисмации Шлика
__device__ float schlick(float cosine, float ref_idx) {
    float r0 = (1.0f-ref_idx) / (1.0f+ref_idx);
    r0 = r0*r0;
    return r0 + (1.0f-r0)*pow((1.0f - cosine),5.0f);
}

// Вычисление преломленного луча (вычисления основаны на законе Снелла)
__device__ bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
        return false;
}

#define RANDVEC3 vec3(curand_uniform(local_rand_state), curand_uniform(local_rand_state) ,curand_uniform(local_rand_state))

// Методом отбрасывания находим координаты рассеивания в точке луча (смотрим рассеялся ли луч)
__device__ vec3 random_in_unit_sphere(curandState *local_rand_state) {
    vec3 p;
    do {
        p = 2.0f*RANDVEC3 - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0f);
    return p;
}

// Функция вычисляющая отраженный луч (простая оптика)
__device__ vec3 reflect(const vec3& v, const vec3& n) {
     return v - 2.0f*dot(v,n)*n;
}

// Класс материалов
class material  {
    public:
        // Метод описывающий поведение луча который столкнулся с поврехностью
        // где  r_in - входной луч
        //      rec - параметры точки объекта в которую попал луч
        //      attenuation - затухание маетриала (по сути возвращемый параметр)
        //      scattered - новый луч получившийся после столкновения с объектом (тоже возвращаемый объект)
        __device__ virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, curandState *local_rand_state) const = 0;
};

// Матовый материал
// albedo - харaктеристика отражательной способности поверхности (ну или затухание)
class lambertian : public material {
    public:
        __device__ lambertian(const vec3& a) : albedo(a) {}

        // Метод показывающий взаимодействие лучей с материалом
        // Данный материал будет случайно рассеивать лучи вблизи с точкой столкновения
        __device__ virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, curandState *local_rand_state) const  {
             // Точка на объекте где рассеялся луч
             vec3 target = rec.p + rec.normal + random_in_unit_sphere(local_rand_state);
             scattered = ray(rec.p, target-rec.p);
             attenuation = albedo;
             return true;
        }

        vec3 albedo;
};

// Класс металлика (отражающий материал)
class metal : public material {
    public:
        // Задаем также f - небольшой коэф который будет задавать нечеткость при отражении
        // тем самым делая поверхность НЕидеально отражающей 
        __device__ metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }

        // Для этого материала лучи будут отражаться с небольшим случайным отклонением
        __device__ virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, curandState *local_rand_state) const  {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(local_rand_state));
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0.0f);
        }
        vec3 albedo;
        float fuzz;
};

// Класс диэлектрика (прозрачный материал, работающий на преломлении лучей)
// P.S Знаю что по заданию не надо было но в книге описывалось неплохо, решил попробовать
class dielectric : public material {
public:
    __device__ dielectric(float ri) : ref_idx(ri) {}
    // Луч в зависимости от свеого
    __device__ virtual bool scatter(const ray& r_in,
                         const hit_record& rec,
                         vec3& attenuation,
                         ray& scattered,
                         curandState *local_rand_state) const  {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        // float reflect_prob;
        float cosine;
        // Проверяем под каким углом попал луч
        // Если меньше 90 градусов к нормали, то преломляем в одну сторону иначе, в другую
        if (dot(r_in.direction(), rec.normal) > 0.0f) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1.0f - ref_idx*ref_idx*(1-cosine*cosine));
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        // if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        //     reflect_prob = schlick(cosine, ref_idx);
        // else
        //     reflect_prob = 1.0f;

        scattered = ray(rec.p, refracted);
        return true;
    }
    // Коеф преломления
    float ref_idx;
};
#endif
