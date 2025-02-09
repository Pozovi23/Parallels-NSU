Для запуска выполните команды:

mkdir task_1_Cmake

cd task_1_Cmake

wget 'https://raw.githubusercontent.com/Pozovi23/Parallels-NSU/refs/heads/CMake/task_1_Cmake/task_1_Cmake.cpp'

wget 'https://raw.githubusercontent.com/Pozovi23/Parallels-NSU/refs/heads/CMake/task_1_Cmake/CMakeLists.txt'

Далее для выбора типа float необходимо в файле CMakeLists.txt в строке target_compile_definitions(task_1_Cmake PUBLIC TYPENAME=*название типа*) убедиться, что TYPENAME=float

ИЛИ

Для выбора типа double необходимо в файле CMakeLists.txt в строке target_compile_definitions(task_1_Cmake PUBLIC TYPENAME=*название типа*) убедиться, что TYPENAME=double

Далее выполните команды:

mkdir build

cd build

cmake ../

make

./task_1_Cmake

----------------------------------------------------------
Вывод float: -0.0277862

Вывод double: 6.27585e-10


Итого: в зависимости от того, сколько бит отводится под одно число (32 - float или 64 - double) получается разная точность. Это происходит из за представления чисел с плавающей точкой в компьютере. Соответственно, при суммировании большого количества чисел суммируется и ошибка, которую мы и наблюдаем.