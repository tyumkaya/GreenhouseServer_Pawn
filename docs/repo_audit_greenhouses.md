# Repo Audit: Greenhouses

Дата аудита: 2026-04-16

## 1. Что уже есть

### Точка входа gamemode
- Основной gamemode: `gamemodes/gamemodes.pwn`.
- Он запускается через `config.json`, секция `pawn.main_scripts`, значение `"gamemodes 1"` (`config.json:104-112`).
- Основные include-ы gamemode: `open.mp`, `streamer`, `a_mysql`, `sscanf2`, `Pawn.CMD`, `Pawn.Regex`, `mdialog` (`gamemodes/gamemodes.pwn:1-14`).

### Конфиг сервера
- Классического `server.cfg` в репозитории нет.
- Сервер настроен через `config.json`.
- Текущие legacy plugins: `mysql`, `streamer`, `pawnregex` (`config.json:105-108`).

### Способ компиляции
- Явного build-скрипта (`.bat`, `.ps1`, `Makefile`) в репозитории нет.
- В проекте есть Qawno и локальный компилятор: `qawno/qawno.exe`, `qawno/pawncc.exe`.
- Практически это означает ручную компиляцию `gamemodes/gamemodes.pwn` через Qawno/pawncc.
- Побочный признак успешной прошлой компиляции уже есть: `gamemodes/gamemodes.amx`, `gamemodes/gamemodes.xml`.
- Вывод: текущая сборка, скорее всего, делается из Qawno, без отдельного automation layer.

### Текущая модульность
- Код уже частично разложен по модулям в `server/*`.
- Базовые общие части:
  - `server/define.inc`
  - `server/settings.inc`
  - `server/variables.inc`
  - `server/main.inc`
- Игроки вынесены в отдельный мини-модуль:
  - `server/players/variables.inc`
  - `server/players/public.inc`
  - `server/players/mysql.inc`
  - `server/players/dialog.inc`
  - `server/players/dialog_response.inc`
  - `server/players/stock.inc`
- Эти части подключаются напрямую из `gamemodes/gamemodes.pwn`, в том числе блоком внизу файла (`gamemodes/gamemodes.pwn:425-430`).

### Streamer plugin/include
- `streamer` plugin есть: `plugins/streamer.dll`.
- `streamer` include есть: `qawno/include/streamer.inc`.
- Include реально подключен в gamemode (`gamemodes/gamemodes.pwn:2`).
- Plugin реально загружается сервером (`config.json:105-108`).
- Вывод: streamer уже доступен и его можно использовать для dynamic objects, pickups, 3D text labels, areas.

### Текущий MySQL слой
- MySQL подключается в `OnGameModeInit` через `mysql_connect` (`server/main.inc:1-11`).
- Хэндл хранится глобально: `new MySQL:dbHandle;` (`server/variables.inc:1`).
- Настройки БД лежат в `server/settings.inc:1-4`:
  - host: `localhost`
  - user: `root`
  - password: `""`
  - database: `"greenhouse"`
- Закрытие соединения делается в `OnGameModeExit` через `mysql_close` (`server/main.inc:14-18`).

### Структура данных игрока
- Текущая структура игрока:
  - `pID`
  - `pName[MAX_PLAYER_NAME]`
  - `pPassword[20]`
  - `bool:pLogin`
- Определена в `server/players/variables.inc:1-7`.
- Доступ идет через макрос `GetPlayer(playerid, field)` (`server/define.inc:1`).

## 2. Как сейчас работает авторизация

### Поток подключения
1. В `OnPlayerConnect` сохраняется имя игрока в `pName` (`gamemodes/gamemodes.pwn`, ранний блок callbacks).
2. Через `SetTimerEx` запускается `TimerConnectToServer`.
3. `TimerConnectToServer` делает:
   - `SELECT id FROM users WHERE name = '%s'`
   - затем вызывает callback `CheckPlayerToBase`
   (`server/players/public.inc:1-8`).
4. `CheckPlayerToBase`:
   - если строк нет, показывает диалог регистрации
   - иначе показывает диалог логина
   (`server/players/mysql.inc:1-11`).

### Регистрация
- В диалоге регистрации пароль валидируется regex-ом `^[a-zA-Z0-9]{6,20}` (`server/players/dialog_response.inc:20-34`).
- После этого вызывается `RegisterPlayer(playerid)` (`server/players/dialog_response.inc:23-27`).
- `RegisterPlayer` делает:
  - `INSERT INTO users (name, password) VALUES (...)`
  - затем сразу вызывает `SpawnGamer(playerid)`
  (`server/players/stock.inc:1-8`).

### Логин
- В диалоге логина выполняется:
  - `SELECT id, password FROM users WHERE name = '%s' AND password = '%s' LIMIT 1`
  - запрос уходит через `mysql_tquery`
  (`server/players/dialog_response.inc:55-58`).
- В `CheckLoginToServer`:
  - если строк нет, пароль считается неверным
  - если строка есть, выставляется `pLogin = true`, затем вызывается `SpawnGamer(playerid)`
  (`server/players/mysql.inc:13-25`).

### Когда игрок считается полностью авторизованным
- Формально текущая точка "игрок залогинен" находится в `server/players/mysql.inc:21`, где выполняется:
  - `GetPlayer(playerid, pLogin) = true;`
- Сразу после этого игроку показывается success-message и вызывается `SpawnGamer(playerid)` (`server/players/mysql.inc:21-24`).
- То есть текущий признак полной авторизации: `pLogin == true`.

### Важное наблюдение
- В ветке регистрации `pLogin` не выставляется в `true`.
- Новый игрок после `RegisterPlayer` спавнится, но флаг логина в показанном коде не устанавливается (`server/players/stock.inc:1-8`).
- Для новой системы это важно: если завязывать доступ к теплицам на `pLogin`, регистрацию надо будет учитывать отдельно или централизовать post-auth flow.

## 3. Как сейчас выполняются SQL-запросы и загрузка игрока

### Текущие SQL-паттерны
- Формирование SQL: `mysql_format(...)`.
- Асинхронные запросы: `mysql_tquery(...)`.
- Синхронный запрос используется в регистрации: `mysql_query(...)`.
- Проверка результата идет через `cache_get_row_count(...)`.

### Что реально загружается о игроке
- Сейчас полноценной загрузки профиля игрока нет.
- По факту система делает только:
  - проверку существования имени в `users`
  - проверку пары `name + password`
  - вставку новой записи при регистрации
- Поля из БД в `InfoPlayer` почти не гидратятся:
  - `pName` заполняется из `GetPlayerName`
  - `pPassword` временно пишется при регистрации
  - `pID` из SQL не сохраняется
  - другие игровые данные вообще не загружаются
- Вывод: текущий MySQL слой очень тонкий и пока не является полноценным data access layer.

## 4. Чего не хватает для системы теплиц

### На уровне кода
- Нет отдельного модуля `server/greenhouses/*`.
- Нет структуры данных теплиц:
  - глобального списка теплиц
  - runtime-состояния объектов/таймеров/areas
  - привязки теплицы к владельцу/доступу
- Нет загрузки/сохранения теплиц из БД.
- Нет слоя прав доступа:
  - владелец
  - список пользователей с доступом
  - админские исключения
- Нет общего post-login hook, куда безопасно подвешивать загрузку всех систем игрока.
- Нет единой инициализации модулей после подключения к БД.

### На уровне БД
- В аудируемом коде используется только таблица `users`.
- Таблиц для теплиц, стадий роста, посаженных культур, прав доступа, состояния объектов и журналов действий нет.

### На уровне геймплея
- Нет зон взаимодействия, интерактивных объектов, 3D-текстов, точек входа/выхода, команд или диалогов для теплиц.
- Нет цикла обновления растений.
- Нет механики сохранения прогресса роста.

## 5. Какие ручные действия потребуются от вас

### Обязательно
- Подтвердить целевую модель теплиц:
  - личные, фракционные, публичные или смешанные
  - сколько теплиц на игрока
  - нужны ли права доступа другим игрокам
  - рост по real time или по server tick
- Подтвердить, можно ли использовать уже имеющийся streamer как основной способ отображения объектов и зон.
- Подтвердить, нужна ли отдельная команда/админ-инструмент для создания и редактирования теплиц в игре.

### Вероятно потребуется
- Добавить/применить SQL-миграции вручную в вашей MySQL БД.
- При необходимости скорректировать `config.json`, если решите добавлять новые plugins.
- Если компиляция у вас делается не через Qawno, показать ваш реальный compile command, чтобы не разъехались include paths и output path.

## 6. Какие действия с БД потребуются от вас

Минимально потребуется создать новые таблицы, например:
- `greenhouses`
- `greenhouse_plots`
- `greenhouse_crops` или `greenhouse_plants`
- `greenhouse_access`
- опционально `greenhouse_logs`

Также потребуется определить:
- связь с `users.id`
- формат хранения координат / interior / world
- как хранится стадия роста
- как хранится last_update_at для offline-progress

Важно:
- текущую `users` таблицу трогать без необходимости не стоит
- текущий login flow уже завязан на `users`
- для `users.id` уже предусмотрено поле `pID`, но сейчас оно не заполняется

## 7. Какие plugins / includes могут понадобиться

### Уже есть и достаточно для базовой версии
- `streamer`
- `a_mysql`
- `sscanf2`
- `Pawn.CMD`
- `Pawn.Regex`
- `mdialog`

### Скорее всего можно обойтись без новых зависимостей
- Для базовой системы теплиц новых plugins не требуется, потому что:
  - объекты и зоны можно сделать через `streamer`
  - сохранение через текущий `a_mysql`
  - команды через `Pawn.CMD`
  - параметры команд через `sscanf2`

### Что может понадобиться опционально
- Отдельный include/helper для итерации или utility-слой, если система разрастется.
- Но на текущем шаге новых обязательных зависимостей я не вижу.

## 8. Где лучше подключать систему теплиц

### Самое безопасное место для нового модуля
- Создать новый каталог:
  - `server/greenhouses/variables.inc`
  - `server/greenhouses/public.inc`
  - `server/greenhouses/mysql.inc`
  - `server/greenhouses/stock.inc`
  - опционально `server/greenhouses/dialog.inc`
  - опционально `server/greenhouses/dialog_response.inc`
  - опционально `server/greenhouses/commands.inc`

### Точки встраивания
- `server/main.inc`
  - после успешного `mysql_connect` загружать статические данные теплиц из БД
  - поднимать streamer-объекты/areas
- `gamemodes/gamemodes.pwn`
  - подключить новый greenhouse-модуль рядом с `server/players/*`
- `OnPlayerConnect`
  - инициализировать runtime-состояние игрока, относящееся к теплицам
- `CheckLoginToServer`
  - это лучший текущий post-auth hook для загрузки данных игрока, связанных с теплицами
- `RegisterPlayer`
  - либо дублировать post-auth bootstrap здесь
  - либо лучше вынести общий `OnPlayerAuthorized(playerid)` / аналогичный flow и вызывать из логина и регистрации
- `OnPlayerDisconnect`
  - сохранять активные изменения, если появится runtime-состояние теплиц у игрока
- `OnGameModeExit`
  - делать финальный flush/cleanup при необходимости

### Почему это лучше, чем встраивать в существующий player-модуль напрямую
- Текущий `server/players/*` отвечает в основном за auth/dialog/mysql для аккаунта.
- Система теплиц логически отдельная.
- Безопаснее держать ее отдельным модулем и использовать только небольшие hooks в auth lifecycle.

## 9. Рекомендация по соблюдению модульной структуры

Да, модульная структура уже есть, и ее лучше соблюдать.

Рекомендуемый стиль:
- каждый крупный subsystem в своей папке под `server/`
- отдельные файлы по ролям:
  - `variables`
  - `public`
  - `mysql`
  - `stock`
  - `dialog`
  - `dialog_response`
  - `commands` при необходимости
- подключение модулей из `gamemodes/gamemodes.pwn`

То есть теплицы лучше не писать одним большим куском в `gamemodes/gamemodes.pwn`.

## 10. Практический вывод

### Что уже готово для теплиц
- Рабочий open.mp gamemode
- Подключенный MySQL plugin/include
- Подключенный streamer plugin/include
- Базовая модульная раскладка кода
- Игроки уже проходят auth flow

### Что мешает начать реализацию без дополнительных решений
- Нет таблиц теплиц в БД
- Нет общего post-auth bootstrap
- Нет полноценной загрузки `users.id` и других данных игрока
- Нет существующего greenhouse module

### Лучшее место интеграции
- Новый модуль `server/greenhouses/*`
- Инициализация модуля в `server/main.inc`
- Загрузка player-related greenhouse data после успешного логина
- Сохранение при disconnect / exit

## 11. Риски и замечания

- В проекте нет `server.cfg`; ориентироваться нужно на `config.json`.
- Логин основан на plaintext password comparison в SQL.
- В регистрации используется синхронный `mysql_query`, а в логине/проверке существования асинхронный `mysql_tquery`.
- `pID` уже предусмотрен, но не используется.
- У нового игрока после регистрации флаг `pLogin` в текущем коде не выставляется, хотя игрок спавнится.

Это не блокирует аудит, но влияет на правильную точку интеграции теплиц.
