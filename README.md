# audupo
### Инструкция для запуска клиента
Необходимо скачать папку Realese и запустить PLayer.exe
***

### Инструкция по настройке сервера 
Руководство по настройке сервера
Установка Apache
Шаг 1
Apache доступен из дефолтных репозиториев Ubuntu, что позволяет устанавливать его с
помощью средств управления пакетами:
1. обновление локального индекса пакетов: ; $ sudo apt update
2. установка пакета apache2: $ sudo apt install apache2
После подтверждения установки apt установит Apache и все необходимые зависимости.
Шаг 2
Настройка файрвола: для работы Apache должен быть открыт 80 порт.
Шаг 3
Настройка виртуальный хостов
Создайте директорию для plist используя флаг -p для создания необходимых
родительских директорий: $ sudo mkdir -p /var/www/plist/html
Далее настройте владельца директории с помощью переменной окружения $USER:
sudo chown -R $USER:$USER /var/www/plist.com/html
Для того, чтобы Apache мог отдавать этот контент, нам необходимо настроить
виртуальный хост с корректными настройками. Cоздадим новый файл для нашего сайта -
/etc/apache2/sites-available/plist.com.conf:
$sudo nano /etc/apache2/sites-available/example.com.conf
Скопируйте следующий текст настроек виртуального хоста в созданный файл:
<VirtualHost *:80>
 ServerAdmin dzangievedc@gmail.com
 ServerName plist.com
 ServerAlias www.plist.com
 DocumentRoot /var/www/plist.com/html
 ErrorLog ${APACHE_LOG_DIR}/error.log
 CustomLog ${APACHE_LOG_DIR}/access.log combined
</VirtualHost>
Сохраните и закройте файл после внесения изменений.
Теперь активируем профиль сайта с помощью утилиты a2ensite:
$ sudo a2ensite plist.com.conf
Деактивируем дефолтный сайт, определённый в 000-default.conf:
$ sudo a2dissite 000-default.conf
Перезапустите Apache для применения внесённых изменений:
$ sudo systemctl restart apache2
Теперь Apache должен работать с вашим доменным именем.
Шаг 4
Перенести XML файл и треки в корневую папку.
Образец редактирования xml файла
1. <audio_group_name>Записать название исполнителя</audio_group_name>
2. <track_name>Записать назание трека</track_name>
3. <time>Записать длительноcть трека</time>
4. <url>Записать название трека, который залит на хостинг в формате .mp3</url>
### Образец редактирования xml файла
1. <audio_group_name>Записать название исполнителя</audio_group_name> 
2. <track_name>Записать назание трека</track_name>
3. "time>Записать длительноcть трека</time"
4. "url>Записать название трека, который залит на хостинг в формате .mp3</url"

