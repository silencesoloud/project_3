#include <iostream>
#include <stdexcept>
using namespace std;

/*
Вокруг считающего стоит N человек, из которых выделен первый, а остальные занумерованы
по часовой стрелке числами от 2 до N. Считающий, начиная с кого-то, ведет счет до M.
Человек на котором остановился счет, выходит из круга. Счет продолжается со следующего
человека и так до тех пор, пока не останется один человек.

Определить
a) номер оставшегося человека, если известно M и то, что счет начинался с первого человека;
b) номер человека c которого начинался счет, если известно M и номер оставшегося человека L.
*/


// Узел односвязного кольцевого списка
struct Node {
    int number;   
    Node* next;   
    Node(int n) : number(n), next(nullptr) {}
};

// Создаёт кольцевой список из N узлов с номерами 1..N
// Возвращает указатель на первый узел (человек №1)
Node* buildCircle(int N) {
    Node* head = new Node(1);
    Node* cur = head;
    for (int i = 2; i <= N; i++) {
        cur->next = new Node(i);
        cur = cur->next;
    }
    cur->next = head; // Замыкаем список в кольцо
    return head;
}

/*
Симулирует задачу Иосифа:
  N       — исходное количество людей
  M       — каждый M-й человек выбывает
  start   — узел, с которого начинается отсчёт
  verbose — если true, выводит пошаговый лог
Возвращает номер оставшегося человека.
Все узлы освобождаются внутри функции.
*/
int simulate(int N, int M, Node* start, bool verbose = false) {
    // Находим узел, стоящий перед start, чтобы удалять через prev->next
    Node* prev = start;
    while (prev->next != start) prev = prev->next;

    if (verbose) {
        cout << "Начальный круг: ";
        Node* tmp = start;
        do {
            cout << tmp->number << " ";
            tmp = tmp->next;
        } while (tmp != start);
        cout << "(счет начинается с " << start->number << ")\n";
    }

    int remaining = N; // Сколько человек ещё в круге
    int step = 1;      // Номер текущего шага для лога

    // Продолжаем, пока в круге больше одного человека
    while (remaining > 1) {
        // Делаем M-1 шаг вперёд: prev встаёт перед тем, кто выбывает
        for (int i = 0; i < M - 1; i++) prev = prev->next;

        Node* toRemove = prev->next; // Человек, который выбывает

        if (verbose) {
            cout << "Шаг " << step << ": выходит человек №" << toRemove->number << "\n";
        }

        // Исключаем узел из кольца и освобождаем память
        prev->next = toRemove->next;
        delete toRemove;
        remaining--;
        step++;

        if (verbose) {
            cout << " Оставшиеся: ";
            Node* tmp = prev->next;
            do {
                cout << tmp->number << " ";
                tmp = tmp->next;
            } while (tmp != prev->next);
            cout << "\n";
        }
    }

    // Последний оставшийся узел
    int survivor = prev->next->number;
    if (verbose)
        cout << "\n>>> Остался человек №" << survivor << "\n";

    delete prev->next; // Освобождаем последний узел
    return survivor;
}

/*
--------------------------------------------------------------------------------------
Часть а) Номер оставшегося человека
Счет начинается с первого человека
Алгоритм: рекурретная формула O(N) http://e-maxx.ru/algo/joseph_problem
J(1) = 0, J(n) = (J(n-1) + M) % n
Результат в 1-индексации: J(N) + 1
--------------------------------------------------------------------------------------
*/
int solvePartA(int N, int M) {
    int pos = 0;
    for (int i = 2; i <= N; i++)
        pos = (pos + M) % i;
    return pos + 1;
}
/*
--------------------------------------------------------------------------------------
Часть б) Номер человека, с которого начинался счет
Известны M и номер оставшегося человека L
Алгоритм: перебор всех начальных позиций с рекурретной формулой \
Для каждой начальной позиции s вычисляем результат и сравниваем с L
--------------------------------------------------------------------------------------
*/
int solvePartB(int N, int M, int L) {
    for (int s = 1; s <= N; s++) {
        Node* circle = buildCircle(N);

        // Находим узел с нужным стартовым номером
        Node* startNode = circle;
        while (startNode->number != s) startNode = startNode->next;

        int survivor = simulate(N, M, startNode, false);
        if (survivor == L) return s; // Нашли подходящую стартовую позицию
    }
    return -1; // Решение не найдено
}

int main() {
    cout << "Выполнил работу: Игнатьев В.Д. РПИб-090304-о25    " << endl;

    int N, M;
    cout << "\nВведите кол-во людей N: ";
    cin >> N;
    cout << "Введите M (счет до M): ";
    cin >> M;

    // Базовая проверка входных данных
    if (N <= 0 || M <= 0) {
        cout << "Ошибка: N и M должны быть положительными!" << endl;
        return 1;
    }

    // Часть а) 
    cout << "\nЧасть а)" << endl;
    cout << "Счет начинается с 1-го человека" << endl;
    int survivor = solvePartA(N, M);
    cout << "Остался человек №" << survivor << endl;

    //  Часть б) 
    cout << "\nЧасть б)" << endl;
    cout << "Найти начальную позицию по известному оставшемуся" << endl;
    int L;
    cout << "Введите номер оставшегося человека L (1-" << N << "): ";
    cin >> L;

    if (L < 1 || L > N) {
        cout << "Ошибка: L должно быть от 1 до " << N << "!" << endl;
        return 1;
    }

    int startPos = solvePartB(N, M, L);
    cout << "Счет начинался с человека №" << startPos << endl;

    //  Проверка результата части б) 
    cout << "\nПроверка" << endl;
    Node* circle = buildCircle(N);
    Node* startNode = circle;
    while (startNode->number != startPos) startNode = startNode->next;
    int check = simulate(N, M, startNode, false);
    cout << "Если начинать с " << startPos << " -> останется " << check << endl;
    cout << "Ожидалось: " << L << endl;
    cout << (check == L ? "Верно!" : "Ошибка!") << endl;

    //  Пошаговая демонстрация (только для небольших N)
    cout << "\nДемонстрация (N=" << N << ", M=" << M << ")" << endl;
    if (N <= 20) {
        Node* demo = buildCircle(N);
        simulate(N, M, demo, true);
    }
    else {
        cout << "(Круг слишком большой для демонстрации)" << endl;
    }

    return 0;
}