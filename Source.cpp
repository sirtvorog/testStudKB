#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

using namespace std;
using namespace std::chrono;


class AlarmSystem {

private:

    double UA;
    double RMV;
    int RDV;
    bool isActive;

    steady_clock::time_point aboveThresholdStartTime;
    steady_clock::time_point belowThresholdStartTime;

    double currentY;
    bool valueProcessed;

public:

    AlarmSystem(double ua, double rmv, int rdv) : UA(ua), RMV(rmv), RDV(rdv), isActive(false), currentY(0.0), valueProcessed(true)
    {
        auto now = steady_clock::now();
        aboveThresholdStartTime = now;
        belowThresholdStartTime = now;
    }

    void processValue(double y) {

        currentY = y;
        valueProcessed = false;
    }

    void run() { 
        cout << "Система запущена. Вводите значения Y(или 'q' для выхода) :" << endl; 

            while (true) {

                if (!valueProcessed) {
                    processCurrentValue();
                    valueProcessed = true;
                }

                checkTimers();
                readInput();
                this_thread::sleep_for(milliseconds(100));
            }
    }

private:

    void readInput() {

        string input;
        cout << "Y = ";
        getline(cin, input);

        if (input == "q" || input == "Q") {

            exit(0);
        }

        if (!input.empty()) {

            try {
                double y = stod(input);
                processValue(y);
            }

            catch (const exception& e) {

                cout << "Ошибка ввода! Введите число или 'q' для выхода." << endl;
            }
            
        }
    }
    void processCurrentValue() {
        auto now = steady_clock::now();

        if (currentY >= UA) {
           
            auto duration = duration_cast<seconds>(now - aboveThresholdStartTime).count();

            if (duration >= RDV && !isActive) {
                isActive = true;
                cout << "Переход в активное состояние. Признак: 1" << endl;
            }
        }
        else {

            aboveThresholdStartTime = now;
        }

        if (currentY < (UA - RMV)) {

            auto duration = duration_cast<seconds>(now - belowThresholdStartTime).count();

            if (duration >= RDV && isActive) {

                isActive = false;
                cout << "Переход в пассивное состояние. Признак: 0" << endl;
            }
        }
        else {

            belowThresholdStartTime = now;
        }

        cout << "Текущее значение: " << currentY << ", Состояние: " << (isActive ? "Активно (1)" : "Пассивно (0)") << endl;
    }

    void checkTimers() {

        auto now = steady_clock::now();

        if (isActive) {

            auto duration = duration_cast<seconds>(now - belowThresholdStartTime).count();

            if (duration >= RDV) {

                isActive = false;
                cout << "Переход в пассивное состояние по таймеру.Признак: 0" <<
                    endl;
            }
        }

        else {

            auto duration = duration_cast<seconds>(now - aboveThresholdStartTime).count();

            if (duration >= RDV) {

                isActive = true;
                cout << "Переход в активное состояние по таймеру.Признак: 1" << endl;
            }
        }
    }
};


int main() {

    setlocale(LC_ALL, "Russian");

    double UA, RMV;
    int RDV;

    cout << "=== Система аварийной сигнализации == = " << endl;
    cout << "Введите значение уставки UA: ";
    cin >> UA;
    cout << "Введите значение гистерезиса RMV : ";
    cin >> RMV;
    cout << "Введите временную задержку RDV(секунды) : ";
    cin >> RDV;

    cin.ignore(1000, '\n');

    AlarmSystem alarm(UA, RMV, RDV);

    alarm.run();

    return 0;
}
