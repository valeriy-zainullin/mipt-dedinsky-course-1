Драйвер-минифильтр для файловой системы (filesystem filter driver).

Патчит исполняемый файл при чтении, и отключает (Да? Отключает?) кеширование файла, чтобы при чтении его им самим, он действительно читался (вдруг в кеше осядет запатченный файл) (может, мы из кеша если достанем, тоже патчим?) (а можно патчить обратно, кстати), и не патчит в этом случае.

Вообще на сайте Майкрософт очень хорошо написано, можно просто по ссылкам походить. Ходил там, удалось зацепиться за некоторые параграфы и статьи. https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/

Сначала нужно было понять, какая архитектура у драйверов в ОС.

Есть 2 вида драйверов-фильтров user-mode и kernel-mode. Я решил делать kernel-mode драйвер, используя KMDF.

Затем наткнулся на видео, где делают то же самое, решил воспользоваться. Там логгировали чтение файла и запрещали доступ к определённому файлу. Мне не подходит, но часть кода взять можно.
Перехватывать чтение не так просто, проще переопределить имя файла, который будет читаться. Это я и сделал, решение нашел в интернете.

Ссылки (в порядке чтения, если начать с конца):
1. https://stackoverflow.com/questions/60169916/how-to-get-current-process-image-path-from-mini-filter-kernel-driver
1. https://community.osr.com/discussion/291361/mini-filter-driver-to-redirect-file-access
1. https://www.youtube.com/watch?v=9rErMsfmK_U&list=PLZ4EgN7ZCzJyUT-FmgHsW4e9BxfP-VMuo&index=11
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_setup_callback
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_filter_unload_callback
1. https://www.google.com/search?q=windows+paged+vs+non-paged+pool (https://superuser.com/a/1411634)
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_registration
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltregisterfilter
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ifs/writing-a-driverentry-routine-for-a-minifilter-driver
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ifs/filter-manager-concepts
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/develop/creating-a-new-filter-driver
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/ifs/#file-system-filter-drivers
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/filter-drivers (помог по задаче понять, что подойдёт драйвер-фильтр, архитектуру).
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/types-of-wdm-drivers
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/design-goals-for-kernel-mode-drivers (цели дизайна драйверов режима ядра)
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/overview-of-windows-components (картинка с архитектурой ОС).
1. https://docs.microsoft.com/en-us/windows-hardware/drivers/develop/
