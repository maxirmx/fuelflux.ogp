# OGP/OGM Encoder → PC817 → Orange Pi Zero 2W: тест подсчёта импульсов (libgpiod)

В комплекте:
- `pulse_test.c` — тестовая программа (C) для подсчёта импульсов по прерываниям (edge events)
- `CMakeLists.txt` — сборка (CMake)
- `README.md` — инструкция

## Схема подключения (1 канал)
**Вход (5V сторона, энкодер):**
- Энкодер: `P1` → модуль PC817: `IN1`
- Энкодер: `GND` → модуль PC817: `G` (вход)

**Выход (3.3V сторона, Orange Pi):**
- PC817 `V1` → GPIO вход Orange Pi (это *сигнал*)
- PC817 `G` (выход) → GND Orange Pi
- Подтяжка (рекомендуется): резистор 4.7 кОм от **3.3V Orange Pi (Pin 1)** к **сигналу V1/GPIO**.

> Важно: 5V на GPIO Orange Pi подавать нельзя. Подтяжку делаем к 3.3V.

## Установка зависимостей
```bash
sudo apt-get update
sudo apt-get install -y gpiod libgpiod-dev
```

## Как найти нужную GPIO line (offset)
Посмотрите список линий:
```bash
gpioinfo
```
Найдите нужную линию (offset) для выбранного физического пина.

Быстрый тест без компиляции:
```bash
gpiomon --falling gpiochip0 <LINE_OFFSET>
```

## Сборка

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Бинарный файл будет в `build/ogp`.

Примечание: `-DCMAKE_BUILD_TYPE=Release` включает оптимизацию -O2 (по умолчанию тоже Release).

## Запуск
### Только импульсы (и pps)
```bash
./ogp gpiochip0 <LINE_OFFSET>
```

### Импульсы + литры + расход (нужно K = импульсов на литр)
```bash
./ogp gpiochip0 <LINE_OFFSET> --k 80
```

Формулы:
- Liters = pulses / K
- Flow (L/min) = (pps * 60) / K

## Калибровка K
1) Пропустите известный объём, например 10.000 L  
2) Посчитайте импульсы P  
3) K = P / 10.000

