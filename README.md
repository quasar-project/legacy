![](https://user-images.githubusercontent.com/75864292/209325057-0b8283c6-6f12-427a-ac44-983377dd8246.png)

### Графический интерфейс для РЛС "Квазар"

#### Функционал программы

- Связь с РЛС по протоколу UDP. Прием телеметрии РЛС и ее обработка. 
- Запуск скриптов на стороне РЛС по команде из программы, таких как формирование радиолокационного изображения с заданными параметрами.
- Загрузчик радиолокационных изображений с РЛС (TCP/IP). Кэширование полученных радиолокационных изображений на диске.
- Автоматическое преобразование радиолокационных изображений в удобный к просмотру формат.
- Интерактивная оффлайн-карта, на которой отображается РЛС, ее диаграмма направленности, траектория полёта и все полученные радиолокационные изображения.
- Инструменты карты, облегчающие работу с изображениями: линейка, маркеры карты, географическая и навигационная информация.
- Возможность работы программы в режиме просмотра радиолокационных изображений из выбранной папки без использования загрузчика.
- Гибкая настройка программы системным администратором.


#### Инструкция по настройке программы и РЛС перед полётом

1. Скачать актуальную версию программы во вкладке релизов репозитория (_например, v2.2.0alpha](https://disk.yandex.ru/d/3Q9cJyLA-VquWA)_). Распаковать архив с программой в удобное место на рабочем ноутбуке.
2. (_Опционально_) Скачать оффлайн-карту нужного места для полёта отсюда: [ссылка](http://uav.radar-mms.com/tiles). Ржевский полигон доступен по [этой ссылке](http://uav.radar-mms.com/tiles/tiles-rzhevkamini.zip).
3. Запустить **Quasar.exe** в папке с СПО и сразу же закрыть.
4. Распаковать оффлайн-карты в папку **app-offline-tiles**, которая появилась в папке с СПО. Структура каталога должна иметь такой вид:![image](https://user-images.githubusercontent.com/75864292/209822784-1cc2c6b0-5f23-45b7-a074-467db155956e.png), то есть не должно быть никаких промежуточных папок.
5. Запустить СПО (**Quasar.exe**). 
6. Открыть вкладку с настройками, нажав на иконку шестеренки в верхней части окна: <br>
![image](https://user-images.githubusercontent.com/75864292/209823033-47b53d87-0204-4ec0-98af-f306012cedc3.png)  <br>
Ввести пароль администратора (**по умолчанию - `123`**).
7. Поставить следующие настройки (_для БВС ВТ-45_): 
- `Использовать карты OpenStreetMap` - снять галочку, если установили оффлайн-карты. Оставить, если не установили и есть подключение к интернету;
- `Включить профайлер` - снять галочку;
- `Включить консоль отладки` - снять галочку;
- `Адрес РЛС` - ввести `192.168.1.48` в первое поле и `9955` во второе (для ВТ-45);
- `Адрес загрузчика` - ввести `192.168.1.10` в первое поле и `10000` во второе (для ВТ-45);
- `Адрес интерфейса формирования РЛИ` - ввести `9845` во второе поле (для ВТ-45);
- `Адрес интерфейса обратной связи с РЛС` - ввести `9846` во второе поле (для ВТ-45);
- Пропустить дальнейшие настройки до пункта `Метаданные в радианах`
- `Метаданные в радианах` - поставить галочку;
- `Учитывать угол сноса при геопривязке` - снять галочку;
- `Коррекция угла расхождения луча` - ввести `5` градусов;
8. Сохранить настройки, нажав кнопку "ОК".
9. Включить РЛС и подключить сетевой кабель к ноутбуку.
10. Открыть Putty. Ввести адрес подключения по SSH: `192.168.1.48`.
11. Ввести логин `user` и пароль `123`.
12. Ввести команды: 
- `cd /navd_com`
- `python3 navd.py`
13. Открыть новое окно Putty. Ввести тот же адрес подключения, логин и пароль.
14. Ввести команды:
- `cd /fsend`
- `python3 fsend.py`
15. Открыть новое окно Putty. Ввести адрес: `192.168.1.47`. Ввести логин: `root`.
16. Ввести команду:
- `cd /execd && ./execd`
- Если не дало результата, попробовать ввести следующие команды:
- `cd /Work/execd`
- `./execd`
17. Открыть СПО, нажать на вкладку сверху **Удаленное соединение с РЛС**, нажать на кнопку **Повторное подключение**.
18. Открыть консоль РЛС (нижняя правая часть окна). Закрепить окно в удобную позицию в программе.
19. Нажать на кнопку снизу окна **Формирование одиночного изображения**. В консоли должен появиться вывод РЛС.
20. Должно появиться изображение (первое формирование может его не дать, но последующие должны). Его можно удалить, нажав на иконку изображения, и на кнопку удаления.
21. Центрировать карту на борте можно при помощи кнопки <br>
![image](https://user-images.githubusercontent.com/75864292/209826304-b920cef3-5b29-4eef-8e22-eaa716a57844.png)
22. Если по каким-то причинам сервис `execd` не работает (не формируется изображение по кнопке), то выполнить следующие действия:
- Открыть новое окно Putty. Подключиться к `192.168.1.47`, логин `root`;
- Ввести команду `mc`;
- Пройти в каталог `Work/readdata/`, запустить `start.py`;
- Формировать изображения при помощи этого скрипта.





