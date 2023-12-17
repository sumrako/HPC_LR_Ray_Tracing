#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

// Класс материала объетов
class material;

// Класс хранящий данные о результатах столкновения луча и объекта
// где  t - рассстониие от начала луча
//      p - координаты точки на объекте
//      normal - вектор нормали
//      material - неожиданно, но материал
struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

// Интерфейс описывающий все объекты в кадре с которыми взаимодействуют лучи
class hitable  {
    public:
        __device__ virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif
