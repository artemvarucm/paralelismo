## Instrucciones

Esta carpeta se puede usar para desplegar un proyecto VSCode de Python ejecutándose localmente en la máquina host, sin contenedor Docker, pero con todas las herramientas y bibliotecas de Python necesarias para las prácticas. Se recomienda desplegar este proyecto sobre Linux.

Para trabajar con el proyecto, se ha de instalar un *virtual environment* de Python en el directorio *.venv* dentro de esta carpeta. Se proporciona un script *install-venv.sh* para crear dicho virtual environment sobre Linux. Ejecutad dicho script desde una terminal para la creación del entorno. **Nota:** Se asume que en el sistema Linux donde desplegamos dicho proyecto, ya esté instalado el software necesario para crear virtual enviroments. Si no es así se puede instalar en Debian/Ubuntu con: `apt install python3-venv`

Una vez creado el *virtual environment*, abriremos dicha carpeta con VSCode y nos aseguraremos que tenemos instaladas las siguientes extensiones de VScode para Python:

1. Python Extension Pack
2. Jupyter

Si no están instaladas, se pueden instalar fácilmente usando la sección de "Extensions" en la barra lateral izquierda de VSCode.

Finalmente, para que todo quede configurado, abrir la paleta de comandos de VSCOde y teclear "Python: Select Interpreter". Seleccionar el intérprete de Python que corresponde al *virtual environment* que se acaba de crear.

