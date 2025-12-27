# Техническое Задание

### Часть 1: Установка соединения
- Создать TCP-сокет
- Подключиться к SMTP-серверу (порт 25)
- Получить и обработать приветствие сервера (220)

### Часть 2: SMTP-диалог
- Отправить HELO/EHLO
- Отправить MAIL FROM
- Отправить RCPT TO
- Отправить DATA
- Передать тело письма
- Завершить точкой
- Отправить QUIT

### Пример SMTP-сессии
```
S: 220 smtp.example.com ESMTP                   C: From: sender@example.com
C: HELO client.example.com                      C: To: recipient@example.com
S: 250 Hello client.example.com                 C: Subject: Test
C: MAIL FROM: <sender@example.com>              C:
S: 250 OK                                       C: Hello! This is test message!
C: RCPT TO: <recipient@example.com>             C: .
S: 250 OK                                       S: 250 OK
C: DATA                                         C: QUIT
S: 354 Send message content                     S: 221 Bye
```

### Пример каркаса кода
```c++
int main() {
    // Отправка HELO
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    char* helo = "HELO myclient.com\r\n";
    write(client_socket, helo, strlen(helo));
    // Подключение к SMTP-серверуread(client_socket, buffer, sizeof(buffer));
    struct sockaddr_in server_addr;printf("%s", buffer);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(25);
    // ... остальные SMTP-команды
    inet_pton(AF_INET, "smtp.example.com", &server_addr.sin_addr);
    close(client_socket);
    connect(client_socket, (struct sockaddr*)&server_addr,
    sizeof(server_addr));
    return 0;
}
// Чтение приветствия 220
char buffer[1024];
read(client_socket, buffer, sizeof(buffer));
printf("%s", buffer);
```

## Критерии оценки и требования к реализации

**Язык программирования:** C/C++ <br>
**Системные вызовы:** socket(), connect(), read(), write(), close()<br>
**Порт:** 25<br>
**Обязательный функционал:**
- Установка TCP-соединения с SMTP-сервером
- Корректная последовательность SMTP-команд
- Обработка ответов сервера (проверка кодов 220, 250, 354)
- Отправка простого текстового письма
- Корректное завершение сессии (QUIT)

**Освобождение ресурсов:** Все сокеты должны быть корректно закрыты

## Тестирование
- **Локальный тестовый сервер:**
  - Поднять локальный SMTP-сервер (например, Python smtpd)
  - Отправить тестовое письмо
- **Публичные SMTP-серверы:**
  - smtp.mail.ru (порт 25)
  - smtp.yandex.ru (порт 25)
- **Проверка в почтовом ящике:**
  - Письмо должно приходить во "Входящие"
  - Или проверять папку "Спам"

## Что необходимо сдать?
- **Исходный код:** Файлы smtp_client.c или smtp_client.cpp.
- Описание использованного SMTP-сервера
- Инструкция по сборке и запуску
- **Примеры работы:**
    - Скриншот полученного письма
    - Вывод клиента в консоли (лог SMTP-сессии)
    - Для продвинутой версии — скриншот письма с вложением