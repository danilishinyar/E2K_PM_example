# Компиляция кода для исполнения в режиме безопасных вычислений на платформе "Эльбрус"

Рассмотрим простейший пример - программу, в которой возможно переполнение буфера.

Для начала скомпилируем в обычном режиме (с флагом `-fno-stack-protector` для наглядности):

```
gcc buffer_overflow_example.c -fno-stack-protector -o buffer_overflow_example
```

```
# file buffer_overflow_example 
buffer_overflow_example: ELF 64-bit LSB executable, MCST Elbrus, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux.so.2, for GNU/Linux 2.6.33, not stripped

```
`lib64` указывает на то, что код собранан для обычного режима исполнения. 
Результат запуска собранного кода:

```
# ./buffer_overflow_example
123
123 - str1
data - str2
```
При переполнении результат работы следующий:

```
# ./buffer_overflow_example
123456789
9 - str1
123456789 - str2
```
Видим, что значение первой строчки переписалось из-за переполнения буфера на стеке.

Теперь скомпилируем в режиме безопасных вычислений:

```
gcc buffer_overflow_example.c -mptr128 -fno-stack-protector -o buffer_overflow_example
```

Также с флагом `-fno-stack-protector`

```
# file buffer_overflow_example
buffer_overflow_example: ELF 64-bit LSB executable, MCST Elbrus, version 1 (SYSV), dynamically linked, interpreter /lib128/ld-linux.so.2, for GNU/Linux 2.6.33, not stripped
```
Ссылка на библиотеку линкера из директории `lib128` указывает на то, что это – код для РБВ.

При переполнении результат работы следующий:

```
# ./buffer_overflow_example
123456789
Ошибка сегментирования
```
Благодаря механизму дескрипторов, в котрых, в отличие от указателей указываются границы выделенной памяти, перепеолнение становится невозможным на аппаратном уровне.

В отладчике:

РБВ:

```
(gdb) run
Starting program: buffer_overflow_example
123456789

Program received signal SIGSEGV, Segmentation fault

exc_array_bounds at 0x502e9a70 ALS2

0x00000000502e9a70 in __vfscanf_internal () from /lib128/libc.so.6
```

Нормальный режим:

```
(gdb) run
Starting program: buffer_overflow_example
123123123
3 - str1
123123123 - str2
[Inferior 1 (process 3234) exited normally]
``` 


### Флаги


* -mptr128 – эта опция указывает компилятору, что сборка производится
для режима РБВ.
* -Wall-protected – по этой опции компилятор выдаст warning’и обо всех несовместимых конструкциях, где дескриптор преобразовывается в
указатель или где выполняется приведение целочисленного значения к
дескриптору.
* -Wreturn-type – по этой опции будут выданы warning’и о функциях, не
выдающих результата.


### Use-after-free
Рассмотрим поведение программы с уязвимостью uaf ([CWE-416: Use After Free](https://cwe.mitre.org/data/definitions/416.html))

```
gcc uaf.c -fno-stack-protector -no-pie -o uaf
```
```
# ./uaf
...
> login
Well done, you're administrator !

```
В обычном режиме программа отрабатывает штантно и мы можем залогиниться в качестве администратора.

```
gcc uaf.c -mptr128 -fno-stack-protector -no-pie -o uaf
```
```
# ./uaf
...
> del
...
Недопустимая инструкция
```
При попытке запустить программу в РБВ, выводитс ошибка, т.к. происходит обращение к подвисшему указателю после free, чтобы избежать этого необходимо обнулять указатель после освобождения памяти.

