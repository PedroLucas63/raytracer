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

### Projeto 02 - Raios e Cameras

- [x] Classe Ray integrada ao fluxo de render.
- [x] Camera ortografica.
- [x] Camera perspectiva com `fovy`.
- [x] Mapeamento pixel -> coordenadas da janela da camera (`screen_window`).
- [x] Frame da camera a partir de `look_from`, `look_at` e `up`.

### Projeto 03 - Intersecao Raio-Objeto

- [x] Estrutura de primitivas (`Primitive`) com `Surfel`.
- [x] Intersecao raio-esfera.
- [x] Registro de materiais e associacao material-objeto.
- [x] Renderizacao por cor do material no ponto de intersecao mais proximo.

### Extras Implementados

- [x] **Objeto plano** (`object type="plan"`).
- [x] **Material quadriculado** (`material type="grid"`) com `color1`, `color2` e `spacing`.
- [x] Background a partir de **imagem** (`background type="image"`).

## Dependencias

Gerenciadas por `vcpkg`:

- `cli11` (CLI)
- `tinyxml2` (parser XML)
- `stb` (leitura/escrita de imagens)
- `catch2` (testes)

Requisitos de ambiente:

- CMake 3.31+
- Compilador com suporte a C++23
- Ninja (preset padrao)

## Uso Com Devcontainer (Recomendado)

Este repositorio pode ser aberto em um devcontainer no VS Code, garantindo ambiente consistente para build e testes.

### 1. Abrir no container

1. Instale a extensao **Dev Containers** no VS Code.
2. Abra a pasta do projeto no VS Code.
3. Execute o comando **Dev Containers: Reopen in Container**.

### 2. Build dentro do container

Com o terminal ja dentro do container:

```bash
cmake --preset default
cmake --build build
```

## Build

### 1. Instalar dependencias

```bash
vcpkg install
```

### 2. Configurar projeto

```bash
cmake --preset default
```

### 3. Compilar

```bash
cmake --build build
```

O executavel principal sera gerado como `build/raytracer`.

## Execucao

Uso basico:

```bash
./build/raytracer <arquivo_cena.xml>
```

Exemplo:

```bash
./build/raytracer scenes/scene12.xml
```

Opcoes de linha de comando:

```text
input_scene_file          Arquivo XML da cena (obrigatorio)
-c, --cropwindow          Janela de crop: x0,x1,y0,y1
-q, --quick               Modo rapido
-o, --output              Sobrescreve arquivo de saida
```

Exemplo com saida customizada:

```bash
./build/raytracer scenes/scene12.xml -o output/minha_cena.png
```

Formatos de saida suportados para `--output`:

- `.png`
- `.jpg` / `.jpeg`
- `.bmp`
- `.tiff`
- `.ppm`
- `.tga`

## Testes

```bash
cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
```

A suite cobre parsing, opcoes de execucao, utilitarios de imagem, `RGBColor`, `Background` e `ParamSet`.

## Formato Dos Elementos Da Cena (XML)

Estrutura geral:

```xml
<RT3>
	<lookat ... />
	<camera ... />
	<film ... />

	<world_begin/>
		<background ... />
		<material ... />
		<object ... />
	<world_end/>
</RT3>
```

### `lookat`

- `look_from="x y z"`
- `look_at="x y z"`
- `up="x y z"`

### `camera`

- `type="orthographic"` ou `type="perspective"`
- `screen_window="l r b t"` (opcional; padrao `-1.555 1.555 -1.0 1.0`)
- `fovy="angulo"` (obrigatorio para perspectiva)

### `film`

- `type="image"`
- Resolucao por `x_res`/`y_res` **ou** `w_res`/`h_res`
- `filename="output/cena.png"`
- `img_type` e `gamma_corrected` podem ser informados no XML

### `background`

Tipos suportados:

- `single_color` com `color`
- `2_colors_l_r` com `l` e `r`
- `2_colors_t_b` com `t` e `b`
- `2_colors_tl_br` com `tl` e `br`
- `2_colors_tr_bl` com `tr` e `bl`
- `4_colors` com `tl`, `tr`, `bl`, `br`
- `image` com `filename`

### `material`

- `type="color"` com `name` e `color`
- `type="grid"` com `name`, `color1`, `color2`, `spacing`

### `object`

- `type="sphere"` com `origin`, `radius`, `material`
- `type="plan"` com `origin`, `norm`, `material`

## Exemplo Completo

```xml
<RT3>
	<lookat look_from="0 10 -5" look_at="0 0 10" up="0 1 0" />
	<camera type="perspective" fovy="30" screen_window="-6 6 -3.5 3.5" />
	<film type="image" w_res="1280" h_res="720" filename="output/scene12.png" img_type="png" />

	<world_begin/>
		<background type="single_color" color="242 242 242"/>

		<material name="grid" type="grid" color1="123 123 123" color2="232 232 232" spacing="2"/>
		<material name="magenta" type="color" color="220 40 180"/>

		<object type="sphere" origin="2.2 2 6.0" radius="2.0" material="magenta" />
		<object type="plan" origin="0 0 0" norm="0 1 0" material="grid" />
	<world_end/>
</RT3>
```

## Estrutura Do Projeto

- `include/Api`: orquestracao da execucao.
- `include/Parser`: parser XML de cenas.
- `include/Scene`: camera, film, scene e background.
- `include/Objects`: primitivas e materiais.
- `include/Image`, `include/Math`, `include/Utils`: bases de suporte.
- `scenes/`: cenas de exemplo.
- `tests/`: testes automatizados.

## Observacoes Importantes

- O render salva a imagem no caminho definido em `film filename` ou sobrescrito por `--output`.
- Caso o arquivo de saida ja exista, o sistema gera um novo nome com sufixo incremental (`-1`, `-2`, ...).
- As opcoes `--quick` e `--cropwindow` ja sao aceitas pela CLI e validadas; a aplicacao direta no render pode ser evoluida em iteracoes futuras.

## Licenca

Este projeto esta sob a licenca definida no arquivo `LICENSE`.