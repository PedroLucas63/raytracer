# Ray Tracer Photon

Implementacao de um ray tracer academico em C++23, desenvolvido na disciplina **DIM0451 - Computer Graphics I**.

O projeto evolui por etapas, cobrindo geracao de imagem, modelos de camera, intersecao raio-objeto e extensoes extras de geometria/material.

## Participantes

- Pedro Lucas Medeiros do Nascimento
- Gabriel Victor da Silva

## Visao Geral

Este ray tracer possui:

- Parser de cenas em XML.
- Suporte a camera ortografica e perspectiva.
- Geracao de imagem com fundo por cor/interpolacao/imagem.
- Intersecao com esfera e plano.
- Sistema de materiais por nome (cor solida e quadriculado).
- Pipeline de render com selecao da intersecao mais proxima.
- Suite de testes automatizados com Catch2.

## Status Das Tarefas (Projetos Da Disciplina)

### Projeto 00/01 - Infraestrutura inicial e Background

- [x] Estrutura base do ray tracer.
- [x] Classe de imagem/film para armazenar pixels.
- [x] Fundo de cor unica.
- [x] Fundo com interpolacao de 2 e 4 cores (incluindo bilinear).
- [x] Leitura de cena XML para camera, film e background.

### Projeto 02 - Raios e Câmeras
- [x] Implementação da estrutura de Raios (`Ray`).
- [x] Câmera Ortográfica.
- [x] Câmera Perspectiva baseada no campo de visão vertical (`fovy`).
- [x] Geração de frame de câmera a partir dos vetores de enquadramento (`look_from`, `look_at`, `up`).
- [x] Mapeamento de pixels na imagem para coordenadas na janela da câmera (`screen_window`).

### Projeto 03 - Interseção Raio-Objeto Básica
- [x] Interface unificada para primitivas geométricas (`Primitive`) e dados de colisão (`Surfel`).
- [x] Teste de interseção raio-esfera analítico e correto.
- [x] Suporte a materiais vinculados individualmente a objetos.
- [x] Renderização Flat da cor dos objetos com base na interseção mais próxima do raio.

### Projeto 04 - Integradores & Materiais
- [x] Arquitetura modular de integradores baseada em herança (`Integrator` e `SamplerIntegrator`).
- [x] Integrador Flat (`RayCastIntegrator` / `Flat`).
- [x] Gerenciamento e reuso de materiais nomeados na cena (`make_named_material` / `named_material`).

### Projeto 05 - Modelo de Reflexão de Blinn-Phong
- [x] Suporte a fontes de luz direcionais (`DirectionalLight`) e pontuais (`PointLight`).
- [x] Componentes de iluminação ambiente, difusa e especular usando a formulação Blinn-Phong.
- [x] Integrador Blinn-Phong completo (`BlinnPhongIntegrator`).

### Projeto 06 - Sombras e Reflexões Perfeitas
- [x] Sombras duras geradas por testes de visibilidade para luzes (`VisibilityTester`).
- [x] Fonte de luz cônica projetora (`SpotLight`).
- [x] Reflexões especulares ideais recursivas (espelhamento perfeito).

### Projeto 07 - Malhas Triangulares (Meshes)
- [x] Primitiva geométrica de Triângulo (`Triangle`).
- [x] Teste de interseção raio-triângulo rápido com obtenção de coordenadas baricêntricas.
- [x] Carregamento automático de malhas triangulares complexas (`TriangleMesh`) a partir de arquivos `.obj` externos (usando a biblioteca `tinyobjloader`).
- [x] Interpolação automática de coordenadas normais por vértice para sombreamento suave.

### Projeto 08 - Estruturas de Aceleração Espacial
- [x] Representação tridimensional de caixas delimitadoras alinhadas aos eixos (`Bounds3` / AABB) com algoritmo rápido de interseção de raios.
- [x] Árvore de partição espacial Bounding Volume Hierarchy (`BVHAccel`) com divisão mediana dos objetos (`split_method="middle"`).

---

## Recursos Extras Implementados (Diferenciais)

Além dos requisitos obrigatórios, os seguintes recursos adicionais e otimizações foram incluídos no Ray Tracer:

1. **Linear BVH (`LinearBVHAccel`)**:
   - Uma estrutura de aceleração espacial otimizada que reorganiza a árvore BVH em um vetor plano contíguo em memória (`_nodes`).
   - Algoritmo de travessia iterativo usando uma pilha de ponteiros de tamanho fixo, eliminando completamente a recursão durante os testes de interseção.
   - Design *cache-friendly* que reduz falhas de cache da CPU, trazendo renderizações muito mais rápidas para cenas complexas com malhas densas.
2. **Renderização Concorrente / Multithreaded (OpenMP)**:
   - Paralelização no laço principal de rendering em nível de linha de pixels usando OpenMP (`#pragma omp parallel for schedule(dynamic, 1)`).
   - Utilização de zonas críticas (`#pragma omp critical`) para atualização e redesenho thread-safe da barra de progresso no terminal.
3. **Toon / Cel Shading**:
   - Material Cel/Toon (`ToonMaterial`) que discretiza o sombreamento difuso em degraus/intervalos bem definidos de tons.
   - Integrador dedicado (`ToonIntegrator`) para renderização estilizada (não-fotorrealista).
4. **Mapeamento de Fundo Esférico (360° / Panorama)**:
   - Suporte ao uso de mapas esféricos em backgrounds (`background type="image"` com atributo `spherical="true"`), onde a direção de raios que não atingem nenhum objeto é mapeada para coordenadas UV de uma imagem panorâmica 360°.
5. **Forma Geométrica Plana (`plan`)**:
   - Primitiva analítica rápida para planos infinitos no espaço 3D, útil para pisos e paredes reflexivas/quadriculadas.
6. **Forma Geométrica Caixa (`box`)**:
   - Primitiva geométrica de caixa (AABB) definida por dois cantos opostos.
7. **Material Quadriculado procedural (`grid`)**:
   - Padrão procedural de xadrez com espaçamento, cor 1 e cor 2 ajustáveis.
8. **Atenuação Física de Luz**:
   - Cálculo de atenuação quadrática e linear em fontes de luz pontuais (`PointLight` e `SpotLight`) a partir de um vetor de atenuação `(c1, c2, c3)` na fórmula $\frac{1}{c_1 + c_2 d + c_3 d^2}$.
9. **Integradores de Diagnóstico**:
   - **`NormalMapIntegrator`**: Mapeia as normais de interseção de superfície dos objetos como cores RGB (ideal para depuração visual de normais e carregamento de malhas).
   - **`DepthMapIntegrator`**: Mapeia a profundidade ($z$) da interseção da câmera ao objeto como uma rampa de escala de cinza configurável por `zmin` e `zmax` (mapa de profundidade/z-buffer).

---

## Dependencias

Gerenciadas automaticamente pelo `vcpkg`:

- `cli11` (Parser de linha de comando)
- `tinyxml2` (Parser de cenas em XML)
- `stb` (Leitura e gravação de arquivos de imagem)
- `catch2` (Estrutura de testes unitários)
- `tinyobjloader` (Carregador de arquivos de modelo OBJ)

Requisitos de ambiente:

- CMake 3.31+
- Compilador C++ com suporte completo a C++23 e OpenMP
- Ninja (preset de build recomendado)

## Uso Com Devcontainer (Recomendado)

Este repositorio pode ser aberto em um devcontainer no VS Code, garantindo ambiente consistente para build e testes.

### 1. Abrir no container

1. Instale a extensao **Dev Containers** no VS Code.
2. Abra a pasta do projeto no VS Code.
3. Use o atalho `Ctrl+Shift+P` e escolha **Dev Containers: Reopen in Container**.

### 2. Build dentro do container
```bash
cmake --preset default
cmake --build build
```

## Build Local

### 1. Instalar dependências e configurar
```bash
vcpkg install
cmake --preset default
```

### 2. Compilar
```bash
cmake --build build
```
O executável principal será gerado no caminho `build/raytracer`.

## Execução

Uso básico:
```bash
./build/raytracer <arquivo_cena.xml>
```

Parâmetros adicionais da linha de comando:
```text
input_scene_file          Arquivo XML contendo a descrição da cena (obrigatório)
-c, --cropwindow          Janela de recorte/crop para renderizar apenas um retângulo: x0,x1,y0,y1
-q, --quick               Modo rápido de renderização rápida
-o, --output              Sobrescreve/especifica o arquivo de saída de imagem
--no-overwrite            Não sobrescreve arquivos existentes. (Default: false)
```

Exemplo:
```bash
./build/raytracer scenes/scene47.xml -o output/cena_teapot.png --no-overwrite
```

Formatos de gravação suportados: `.png`, `.jpg`, `.bmp`, `.tga`, `.ppm`, entre outros.

## Testes

```bash
cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
```

A suite cobre parsing, opcoes de execucao, utilitarios de imagem, `RGBColor`, `Background` e `ParamSet`.

## Formato Dos Elementos Da Cena (XML)

### Estrutura Geral da Cena
```xml
<RT3>
   <lookat look_from="x y z" look_at="x y z" up="x y z" />
   <camera type="orthographic|perspective" fovy="graus" screen_window="l r b t" />
   <film type="image" w_res="largura" h_res="altura" filename="caminho" img_type="png" gamma_corrected="yes|no" />
   <integrator type="flat|blinn_phong|depth_map|normal_map|toon" ... />
   <aggregator type="list|bvh|lbvh" ... />

   <world_begin/>
      <background type="single_color|2_colors_l_r|2_colors_t_b|2_colors_tl_br|2_colors_tr_bl|4_colors|image" ... />
      
      <!-- Definição de materiais -->
      <make_named_material name="nome" type="color|grid|blinn|toon" ... />
      
      <!-- Luzes da cena -->
      <light_source type="ambient|directional|point|spot" ... />
      
      <!-- Objetos -->
      <object type="sphere|plan|box|trianglemesh" ... />
   <world_end/>
</RT3>
```

### Detalhamento das Tags

#### `integrator`
Configura como a cor de cada pixel é computada:
- `type="flat"`: Renderização flat sem iluminação.
- `type="blinn_phong"`: Iluminação Phong clássica com suporte a sombras e reflexões recursivas (espelho).
- `type="depth_map"`: Renderiza a profundidade em escala de cinza. Atributos opcionais: `zmin`, `zmax`, `near_color`, `far_color`.
- `type="normal_map"`: Mapeia as normais do surfel no espaço 3D para cores RGB.
- `type="toon"`: Sombreamento estilizado Cel-Shading.

#### `aggregator`
Define a estrutura de aceleração espacial para interseções:
- `type="list"`: Lista linear simples (busca linear, $O(N)$).
- `type="bvh"`: Árvore Bounding Volume Hierarchy clássica. Atributos: `split_method="middle"`, `max_prims_per_node="4"`.
- `type="lbvh"`: Linear BVH baseada em array contíguo e travessia iterativa altamente otimizada (LBVH). Atributos equivalentes aos da BVH.

#### `background`
- `type="single_color"` com `color="r g b"` (valores 0-255 ou 0-1.0).
- `type="4_colors"` com cantos `tl`, `tr`, `bl`, `br`.
- `type="image"` com `filename="imagem.png"` e opcional `spherical="true"` para imagens panorâmicas de ambiente 360°.

#### `make_named_material` / `material`
- `type="color"`: Cor plana simples. Atributo: `color`.
- `type="grid"`: Procedural quadriculado. Atributos: `color1`, `color2`, `spacing`.
- `type="blinn"`: Sombreamento físico Blinn-Phong. Atributos:
  - `diffuse="r g b"`: Coeficiente de reflexão difusa.
  - `specular="r g b"`: Coeficiente de reflexão especular.
  - `ambient="r g b"`: Coeficiente de reflexão ambiente.
  - `glossiness="valor"`: Brilho especular (exponencial de Phong).
  - `mirror="r g b"`: Coeficiente de reflexão especular ideal (material espelhado recursivo, opcional).
- `type="toon"`: Cel shading. Atributos: `color` e `n_intervals` ou `color_map`.

#### `light_source`
- `type="ambient"`: Luz ambiente uniforme. Atributo: `I="r g b"`.
- `type="point"`: Luz pontual omnidirecional. Atributos: `from="x y z"`, `I="r g b"`, `attenuation="c1 c2 c3"` (padrão `1 0 0`).
- `type="directional"`: Luz direcional paralela. Atributos: `from="x y z"` (direção da luz em direção à origem), `to="x y z"`, `I="r g b"`.
- `type="spot"`: Holofote cônico. Atributos: `from="x y z"`, `to="x y z"`, `I="r g b"`, `cutoff="angulo_graus"` (ângulo de abertura), `falloff="angulo_graus"`, `attenuation="c1 c2 c3"`.

#### `object`
- `type="sphere"`: Esfera tridimensional. Atributos: `center="x y z"` ou `origin="x y z"`, `radius="valor"`, `material="nome"`.
- `type="plan"`: Plano infinito. Atributos: `origin="x y z"`, `norm="x y z"`, `material="nome"`.
- `type="box"`: Caixa AABB. Atributos: `first_point="x y z"`, `second_point="x y z"`, `material="nome"`.
- `type="trianglemesh"`: Malha triangular carregada de arquivo OBJ. Atributos:
  - `filename="scenes/model.obj"`: Caminho para o modelo 3D.
  - `compute_normals="true|false"`: Calcula as normais geometricamente caso o arquivo OBJ não as possua.
  - `backface_cull="true|false"`: Ativa descarte de faces traseiras.
  - `material="nome"`.

---

## Exemplo Completo de Cena (XML)

O exemplo a seguir ilustra o uso da estrutura avançada de aceleração Linear BVH, iluminação Blinn-Phong com sombras, reflexão recursiva metálica em uma malha OBJ e plano quadriculado:

```xml
<RT3>
   <lookat look_from="0 45 10" look_at="0 3 5" up="0 0 1" />
   <camera type="perspective" fovy="45" />
   <integrator type="blinn_phong" depth="3" />
   <film type="image" w_res="900" h_res="700" filename="output/scene47.png" img_type="png" />
   <aggregator type="lbvh" split_method="middle" max_prims_per_node="4" />

   <world_begin/>
      <background type="single_color" color="245 245 245"/>
      
      <!-- Materiais -->
      <make_named_material name="stone" type="blinn" ambient="0.03 0.03 0.03" diffuse="0.22 0.22 0.22" specular="0.05 0.05 0.05" glossiness="8" />
      <make_named_material name="metal" type="blinn" diffuse="0.05 0.05 0.05" specular="0.3 0.3 0.6" ambient="0.5 0.5 0.8" glossiness="512" mirror="0.7 0.7 0.7" />
      <make_named_material name="grid" type="grid" color1="200 200 200" color2="100 100 100" spacing="2"/>

      <!-- Fontes de Luz -->
      <light_source type="point" from="5 15 15" I="0.9 0.9 0.9" attenuation="1 0.05 0.005" />
      <light_source type="directional" from="10 20 -15" to="0 0 7" I="0.4 0.4 0.6" />

      <!-- Cenário e Malha de Triângulos (OBJ) -->
      <object type="plan" origin="0 -10 0" norm="0 1 0" material="grid" />
      <object type="plan" origin="0 0 25" norm="0 0 1" material="stone" />
      <object type="trianglemesh" filename="scenes/obj/teapot-low.obj" compute_normals="false" backface_cull="false" material="metal" />
   <world_end/>
</RT3>
```

## Estrutura do Projeto

- `include/Api` / `src`: Ponto de entrada e orquestração do programa.
- `include/Core`: Estrutura de dados genéricas, `ParamSet` para atributos.
- `include/Parser`: Parser XML genérico e mapeamento de tags para objetos C++.
- `include/Scene`: Gerenciador da câmera (ortográfica/perspectiva), film/film buffer e backgrounds.
- `include/Objects/Aggregate`: Estruturas de agregação linear (`PrimitiveList`), recursiva (`BVHAccel`) e contígua (`LinearBVHAccel`).
- `include/Objects/Shapes`: Definições geométricas: Esferas, Planos, Caixas e Malhas/Triângulos.
- `include/Objects/Materials`: Modelos de sombreamento (Flat, Blinn-Phong, Cel/Toon e texturas procedurais).
- `include/Objects/Light`: Fontes de luz (ambiente, direcional, pontual e spot).
- `include/Integrator`: Execução do pipeline de renderização (Flat, Blinn-Phong, Toon, Mapas de profundidade/normais).
- `include/Math`: Classes algébricas básicas: vetores, pontos, raios, eixos e caixas delimitadoras (`Bounds3`).
- `include/Utils`: Elementos utilitários de suporte, como a barra de progresso em terminal.

## Licença

Este projeto está licenciado sob os termos do arquivo `LICENSE`.