# Descrição

O objetivo principal desse projeto-demonstração é **criar toda a infraestrutura
necessária para armazenar uma malha de triângulos**, representada como uma
estrutura de dados do tipo **Malha Indexada de Triângulos** (MIT)[^1]. Para
mais detalhes técnicos, recomendo a leitura da Seção 12.1.2 do Capítulo 12 do
livro _"Fundamentals of Computer Graphics" 4th ed_, S. Marschner and P.
Shirley, disponível [aqui](../doc/FoCG_sec_12.1.2.pdf).
[^1]: Em Inglês: _Indexed Triangle Mesh_

Recorde que, de acordo com a descrição do Projeto 07, as informações sobre uma
MIT podem vir especificadas _(1)_ diretamente no arquivo de cena OU _(2)_
fornecidas indiretamente via um **arquivo no formato [Wavefront obj
file](https://en.wikipedia.org/wiki/Wavefront_.obj_file)**, o qual deve ser
processado para extrair as informações necessárias para criar a MIT
correspondente. Para viabilizar a leitura e processamento de arquivo '.OBJ'
esse projeto utiliza a API
[tinyobjloader](https://github.com/tinyobjloader/tinyobjloader).

O programa lê e imprime na tela os dados lidos de um arquivo '.OBJ' especificado no `main.cpp`. Obviamente, a parte de impressão pode ser completamente removida se você incorporar esse código ao seu projeto. A impressão das informações na saída padrão foi feita apenas para depurar o código e, ao mesmo tempo, faciliar a visualização da forma que os dados da MIT são estruturados e organizados na memória e em estrutura de dados.

O que você precisa fazer a partir desse código-exemplo é:

1. Implementar as funções de _interseção_ para o triângulo, dentro da classe `Triangle` que deriva da classe `Shape`.
2. Realizar o _parser_ dos atributos do tag `<object type="trianglemesh" ...\>` quando esses atributos são especificados diretamente no arquivo de cena (e não lidos de um arquivo '.OBJ'). A sugestão aqui é que os dados extraídos do arquivo de cena sejam incorporados à mesma estrutura de dados que foi utilizada para ler os dados de um arquivo '.OBJ', de maneira unificar o processo de criação de uma MIT. Isso deve ser feito dentro da função `create_triangle_mesh_shape()`.
3. Implementar o _cálculo de normais_ no momento da leitura da malha. Esse cálculo será necessário em 2 situações, a saber: quando o arquivo '.OBJ' ou arquivo de cena não contém indicação de normais OU quando o cliente solicita o cálculo explicitamente, ao definir o atributo `compute_normals="true"`.
4. Incorporar esse código ao seu projeto, se assim desejar.

# Componentes do Projeto

- Pasta [`data`](data/): contém alguns arquivos '.OBJ' de exemplo.
- Pasta [`ext`](ext/): contém os arquivos da API tinyobjloader necessários para compilar o projeto (não é necessário instalar nada no sistema).
- Pasta [`src`](src/): arquivos fontes desse projeto.
- Arquivo [`main.cpp`](src/main.cpp): simula o _ray tracer_, mais especificamente a parte que cria o `ParamSet` e chama a função para criar a malha.
- Arquivo [`rt3.h`](src/rt3.h): algumas classe (vazias), para tornar o código mais próximo do final, como `ParamSet`, `Point3f`, `Point2f`, `Normal3f`, `Shape` etc.
- Arquivo [`triangle.h`](src/triangle.h): Descrição das classes `Triangle` and `TriangleMesh` + funções auxiliares.
- Arquivo [`triangle.cpp`](src/triangle.cpp): Implementação dos métodos e funções. Aqui acontece a leitura do arquivo '.OBJ' de fato.

# Compilando com cmake

Da pasta raiz do projeto faça:

```
cmake -S source -Bbuild
cmake --build build
```

Pra executar:

```
./build/load_mesh_demo
```

# Autoria

&copy; 2020-2026, Dimap, Selan R. Santos.
