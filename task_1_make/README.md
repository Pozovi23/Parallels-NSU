Для запуска выполните команды:

mkdir task_1_make

cd task_1_make

wget 'https://raw.githubusercontent.com/Pozovi23/Parallels-NSU/refs/heads/Make/task_1_make/task_1_make.cpp'

wget 'https://raw.githubusercontent.com/Pozovi23/Parallels-NSU/refs/heads/Make/task_1_make/Makefile'

Далее для выбора типа float необходимо в файле task_1_make.cpp
  1) Раскомментировать строку #define TYPENAME float
  2) Закомментировать строку #define TYPENAME double

ИЛИ

Для выбора типа double соответственно
  1) Раскомментировать строку #define TYPENAME double
  2) Закомментировать строку #define TYPENAME float

Далее выполните команду:

make


Вывод float: -0.0277862

Вывод double: 4.89582e-11


Итого: в зависимости от того, сколько бит отводится под одно число (32 - float или 64 - double) получается разная точность. Это происходит из за представления чисел с плавающей точкой в компьютере. Соответственно, при суммировании большого количества чисел суммируется и ошибка, которую мы и наблюдаем.
