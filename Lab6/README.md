A)As tarefas respeitam seus período  possuindo escalonamento (50ms - time-slice), 
Os leds ficam piscando alternadamente.

B)As tarefas esperam a tarefa que esta sendo executada equanto o processador livera de acordo com a prioridade. 
Com isso os leds não ficam alternando entre si  e sim esperando o anterior terminar a tarefa.

C)Como o led com mais prioridade interrompe a execução do atual para iniciar a sua tarefa eles se alternam.
Fazendo com que os periodos das threads alternem também.

D)Com o Mutex, os períodos de execução de cada thread(1 e 3) varia, pois a thread com mutex tera seu tempo aumentado. 

E)Similar ao caso acima, com a thread 2 possuindo herança com o mutex,o que pode fazer com que seu periodo aumente