# Phonebook

Версия ядра - 6.1.9

## Task

Необходимо создать модуль ядра, с которым можно взаимодействовать через символьное устройство. Модуль должен реализовывать простой телефонный справочник. Справочник для каждого пользователя хранит имя, фамилию, возраст, номер телефона, электронную почту.

Необходимо:
- уметь возвращать все данные по заданной фамилии пользователя;
- уметь добавлять нового пользователя;
- уметь удалять заданного пользователя.

Необходимо разработать и зарегистрировать в ядре системные функции, которые упрощают взаимодействие с символьным устройством, созданным в первом задании. Должно быть реализовано три функции:

- long get_user(const char* surname, unsigned int len, struct user_data* output_data);
- long add_user(struct user_data* input_data);
- long del_user(const char* surname, unsigned int len).

## Guide

**Files:**
1. phonebook.c - исходный код
2. Makefile - файл для сборкии модуля

**Usage guide:**
1. Добавление модуля - insmod /path/to/phonebook.ko
2. Добавление символьного устройства - mknod sym_name c <major_number> 0
3. Функционал модуля:
  - Add user - echo "name surname age email phone" > sym_name
  - Get user - echo "surname" > sym_name && cat sym_name
  - Del user - echo "del surname" > sym_name
4. Удаление модуля - rmmod /path/to/phonebook.ko

Рабоий процесс модуля сопровождается выводом логов о (не)успехе взаимодействия с символьным устройством в dmesg
