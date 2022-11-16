# Air Raid Alert на ESP8266
<b>Проект персонального оповещателя о воздушных тревогах для жителей Украины.</b><br><br>
Основан на работе с API sirens.in.ua.<br>
В скетче задаётся область для которой будет совершаться мониторинг состояния тревог.<br>
В обычном режиме светодиод светиться зеленым светом, а при наступлении тревоги будет раздаваться короткая сирена и на протяжении всей тревоги светиться красный светодиод. Если в области объявлена частичная тревога, то красный светодиод будет моргать. По завершению тревоги светодиод снова загорается зеленым и звучит короткий отрывок гимна Украины.<br>
<br>
<b>Список компонентов для проекта:</b><br>
📌 WeMos D1 Mini<br>
📌 Пьезоизлучатель (Buzzer)<br>
📌 Светодиод с общим катодом<br>
📌 N-chanel MOSFET 2N7000 (или анологичный)<br>
📌 резистор 100Ом - 2шт<br>
📌 резистор 10кОм - 1шт<br>
<br>
<b>Схема подключения:</b><br>
<img src="https://raw.githubusercontent.com/pavel-fomychov/air-raid-alert-esp8266/main/scheme.jpg"><br>
<br>
<b>Макет:</b><br>
<div style="text-align:center;">
<center>
<img src="https://raw.githubusercontent.com/pavel-fomychov/air-raid-alert-esp8266/main/miniature.jpg" style="text-align:center;">
</center>
</div>
<br>
<br>
📌 Гайд по добавлению плат esp8266 в Arduino IDE:<br>
https://arduinomaster.ru/platy-arduino/proshivka-esp8266-cherez-arduino-ide/<br>
<br>
📌 Гайд по установке библиотек:<br>
https://www.arduino.cc/en/Guide/Libraries<br>
