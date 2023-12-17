# HPC Ray Tracing
Это приложение представляет собой реализацию трассировщика лучей (ray tracer) с использованием CUDA для параллельных вычислений на GPU. Программа генерирует изображение сцены, состоящей из сфер, с учетом различных материалов и освещения. Многие идеи были подчерпнуты из книги [Ray Tracing on Weekend](http://in1weekend.blogspot.com/2016/01/ray-tracing-in-one-weekend.html).

## Запуск:
Данные команды создадут изображения в out.ppm и out.bmp.
### На `Windows`
``` bash
make win_out.ppm
```

### На `Linux`
``` bash
make linux_out.ppm
```

## Результаты
![Получившееся изображение](https://github.com/sumrako/HPC_LR_Ray_Tracing/blob/master/out.bmp)
Итоговое изображение будет сохранено в файле out.bmp и out.ppm.

## Оценка времени работы
Среднее время выполнения программы у автора описанной в заглавии [книги](http://in1weekend.blogspot.com/2016/01/ray-tracing-in-one-weekend.html) составило около **90 секунд** на чистом C++, на процессоре *Intel Core i7 (6 ядер)*  для данного изображения: 
![изображения](https://github.com/sumrako/HPC_LR_Ray_Tracing/blob/master/out.jpg)
В моей реализации на основе cuda для создания данного изображения понадобилось **1.3 секунды** на видеокарте *Nvidia RTX 4060 Ti*.
