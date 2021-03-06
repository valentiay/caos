# caos

## files-and-fs

### wc

wc "список файлов"
Два варианта:

• с однобайтными символами (2 балла): см. isspace

• с многобайтными (4 балла): см. mbrtowc, iswspace, L'\n'
Вывести информацию о содержимом файлов: число строк, слов и символов.

• Строки: последовательность до перевода строки ('\n' или L'\n').

• Слова: непустая последовательность непробельных символов

• Символы: суммарно все символы, включая переводы строк и т.д.
Пример:

• "a\n" — одна строка, одно слово, два символа

• "a" — ноль строк, одно слово, один символ
Вывод:

• если файлов нет (stdin), то просто три числа

• если файл один, то три числа и имя файла

• если файлов больше одного: по каждому файлу — как в предыдущем, дополнительно
суммарная статистика (total)

### tail

tail "список файлов"
Выводит 10 последних строк из каждого файла или stdin.
Если файлов больше одного, то в начале — имя файла.
Если строк меньше 10 — сколько есть.

### ls

Работает как ls -alSR "список объектов"
Т.е. получает на вход имя директории/файла (если нет, значит текущая) и рекурсивно
выводит информацию о файлах (имя пользователя, группу, размер и т. д.)
Список отсортирован по размеру, в первой группе должны различаться:

• обычные файлы (-)

• директории (d)

• ссылки (l)

Оценка: 3-5

• 3: есть рекурсивный обход (-aR)

• +1: есть информация о файле (-l)

• +1: отсортировано по размеру (-S)

◦ размер для директорий и файлов: то что вернет stat

## pipes

Программа pipes: pipes имена программ
./pipes pr1 pr2  ... prN

Запустить параллельно программы pr1 ... prN (fork,execlp), связать их
каналами (pipe, dup2). Т.е. получить аналог в shell:
pr1 | pr2 | ... | prN

## eratothenes

Условие: реализовать параллельный алгоритм решета Эратосфена 

Программа получает на вход два параметра:
argv[1] — число нитей
argv[2] — максимальное число решета (если отсутствует, то бесконечность)

Эффективность программмы можно проверять с помощью утилиты time.

Вывести список простых чисел, разделенных переводом строки (по одному в строке).

## grep

Задачи на межпроцессное взаимодействие и ФС

Общая схема: породить несколько процессов, каждому из которых дать свою часть задачи.
То, что в больших системах называется map/reduce

Для выполнения отдельной части задачи можно использовать уже написанный вами код или стандартную утилиту.
Встроенные в утилиты методы распараллеливания использовать нельзя.

Связь между отцом и сыновьями:
a) pipe
* poll/select позволят эффективнее обрабатывать дескрипторы
* pipe и временные файлы — единственный вариант с внешними программами
b) shared memory + сигнал об окончании (wait/sigaction/…)
c) очереди сообщений

Временные файлы создают с помощью mkstemp.

Эффективность параллельности можно оценивать через time. 
Оптимальное число может быть больше числа ядер, т.к. процесс тратит время чтение/запись, т.е. не является чисто счетным.

Пример существующей утилиты:
* make, можно задать число параллельных компиляций через ключ -j. Компиляция задается через shell-подобную строку, т.е. является произвольной программой с параметрами
* GNU sort

Задачи:

2. grep на тысячи имен

Программе дается три параметра: образец строки, файл с именами и число параллельно работающих сыновей.
Каждый сын реализует grep (через ваш код с простым шаблоном или exec) на один файл. 
Родитель должен отобрать уникальные строки.

## network

Дополнительные знания: распараллеливание/select/poll/…

Аналогично предыдущему варианту, но:
* несколько клиентов обрабатываются параллельно (5)
