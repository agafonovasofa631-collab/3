#include <iostream>
#include <stdexcept>
#include <utility>  // Для std::move (перемещение ресурсов)
#include <iterator> // Для random_access_iterator_tag (тег итератора)

using namespace std;


template <typename T>
class Vector {
private:
    T* data;    // Указатель на начало массива в памяти
    size_t sz;  // Size (размер) - сколько элементов лежит в массиве
    size_t cap; // Capacity (емкость) - сколько элементов может поместиться без расширения

public:
    
    // Вложенный класс итератора (Random Access)
   
    class Iterator {
    public:
        using iterator_category = random_access_iterator_tag; // Самый мощный тип итератора
        using value_type = T;                          // Тип элементов
        using difference_type = ptrdiff_t;                  // Тип для разности итераторов
        using pointer = T*;                         // Указатель
        using reference = T&;                         // Ссылка

    private:
        pointer ptr; // Внутри итератор хранит обычный указатель на текущий элемент

    public:
        // Конструктор
        Iterator(pointer p = nullptr) : ptr(p) {}

        // Разыменование
        // *it - получить значение элемента
        reference operator*() const { return *ptr; }
        // it->field - получить доступ к полям объекта
        pointer operator->() const { return ptr; }

        // Инкремент и декремент (Сдвиг на 1) 
        // ++it (сначала сдвиг, потом возврат)
        Iterator& operator++() { ptr++; return *this; }
        // it++ (сначала возврат копии, потом сдвиг)
        Iterator operator++(int) { Iterator temp = *this; ptr++; return temp; }
        // --it
        Iterator& operator--() { ptr--; return *this; }
        // it--
        Iterator operator--(int) { Iterator temp = *this; ptr--; return temp; }

        // Арифметика (Сдвиг на n шагов) 
        // it + 2
        Iterator operator+(difference_type n) const { return Iterator(ptr + n); }
        // it - 2
        Iterator operator-(difference_type n) const { return Iterator(ptr - n); }
        // Расстояние между итераторами (it2 - it1)
        difference_type operator-(const Iterator& other) const { return ptr - other.ptr; }
        // it += 2
        Iterator& operator+=(difference_type n) { ptr += n; return *this; }
        // it -= 2
        Iterator& operator-=(difference_type n) { ptr -= n; return *this; }

        // Сравнения
       
        bool operator==(const Iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        bool operator<(const Iterator& other) const { return ptr < other.ptr; }
        bool operator>(const Iterator& other) const { return ptr > other.ptr; }
        bool operator<=(const Iterator& other) const { return ptr <= other.ptr; }
        bool operator>=(const Iterator& other) const { return ptr >= other.ptr; }
    };

    // Правило пяти

    // 1) Конструктор по умолчанию. Создает пустой вектор.
    Vector() : data(nullptr), sz(0), cap(0) {}

    // 2) Деструктор. Вызывается при удалении объекта.
    ~Vector() {
        delete[] data;
    }

    // 3) Конструктор копирования (Глубокое копирование)
    Vector(const Vector& other) : sz(other.sz), cap(other.cap) {
        data = new T[cap]; // Выделяем новую память для копии
        for (size_t i = 0; i < sz; ++i) {
            data[i] = other.data[i]; // Копируем элементы по одному
        }
    }

    // 4) Конструктор перемещения 
    Vector(Vector&& other) noexcept
        : data(other.data), sz(other.sz), cap(other.cap) {
        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    // 5) Оператор присваивания копированием 
    Vector& operator=(const Vector& other) {
        if (this != &other) { // Проверка на самоприсваивание (v = v)
            Vector temp(other); // Создаем временную копию
            swap(data, temp.data); // Меняемся внутренностями
            swap(sz, temp.sz);
            swap(cap, temp.cap);
        }
        return *this;
    }

    // 6) Оператор присваивания перемещением
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data; // Сначала очищаем свои старые данные

            // Забираем чужие
            data = other.data;
            sz = other.sz;
            cap = other.cap;

            // Обнуляем источник
            other.data = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    // Основные методы

    // Добавление элемента в конец
    void push_back(const T& value) {
        // Если места нет (размер равен емкости)
        if (sz >= cap) {
            size_t newCap = (cap == 0) ? 1 : cap * 2; // Увеличиваем емкость в 2 раза
            reserve(newCap); // Выделяем новую память и перекладываем старые данные
        }
        data[sz++] = value; // Записываем новый элемент и увеличиваем размер
    }

    // Изменение емкости (выделение памяти под newCap элементов)
    void reserve(size_t newCap) {
        if (newCap <= cap) return;

        T* newData = new T[newCap]; // Выделяем новый блок
        for (size_t i = 0; i < sz; ++i) {
            newData[i] = move(data[i]); // Перемещаем старые данные в новый блок
        }

        delete[] data; // Старый блок удаляем
        data = newData; // Сохраняем указатель на новый
        cap = newCap; // Обновляем емкость
    }

    // Получение размера и емкости
    size_t size() const { return sz; }
    size_t capacity() const { return cap; }

    // Итераторы на начало и конец. 
    Iterator begin() { return Iterator(data); }
    Iterator end() { return Iterator(data + sz); }
};


int main() {
    setlocale(LC_ALL, "ru");

    cout << "=== Тестирование Vector и Rule of Five ===\n";

    Vector<int> v1;
    v1.push_back(10);
    v1.push_back(20);
    v1.push_back(30);

    // 1. Проверка конструктора копирования (копируем v1 в v2)
    Vector<int> v2 = v1;

    // 2. Проверка конструктора перемещения (перемещаем v1 в v3)

    Vector<int> v3 = move(v1);

    // Выводим v2 с помощью обычного цикла и итераторов
    cout << "v2 (копия): ";
    for (auto it = v2.begin(); it != v2.end(); ++it) {
        cout << *it << " "; 
    }
    cout << "\n";

    // Выводим v3 через range-based for 
    cout << "v3 (перемещенный): ";
    for (int val : v3) {
        cout << val << " ";
    }
    cout << "\n";

    // 3. Проверка Random Access Iterator
    auto it = v3.begin();

    // Проверяем, что можем прыгать через элементы (it + 1)
    cout << "Элемент по индексу 1 (через итератор it + 1): " << *(it + 1) << "\n";

    // Проверяем, что можем находить расстояние между итераторами
    cout << "Разница между итераторами (end - begin): " << (v3.end() - v3.begin()) << "\n";

    return 0;
}
