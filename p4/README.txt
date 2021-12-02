# Computa��o Gr�fica - Trabalho Pr�tico Parte 4
# Alunos: Dimitre Ortt & Willian Ferreira
# Professor: Paulo Pagliosa

#-----------------------------------------##------------------------------------------#
A1 - Implementamos na classe RayTracer o m�todo recursivo
	
	void hierarchyIntersection(const Ray& ray, Intersection& hit, SceneObject* o);

o qual � o respons�vel por verificar a intersec��o de um raio com os objetos da
hierarquia da cena. Caso exista alguma intersec��o, ele atribui o primitivo do objeto
cuja intersec��o � a mais pr�xima ao atributo hit.object, al�m das informa��es sobre o
tri�ngulo (mais pr�ximo) interceptado. Este m�todo funciona verificando a intersec��o 
de um raio com cada objeto de cena e com seus filhos recursivamente atrav�z do m�todo
Primitive::intersect().

#-----------------------------------------##------------------------------------------#
A2 - Substituimos na classe Primitive a implementa��o do m�todo Primitive::intersect() 
com a nossa implementa��o da intersec��o de um raio com a malha de tri�ngulos do 
primitivo. Nossa implementa��o funciona acessando-se cada tri�ngulo da malha e 
calculando a intersec��o do raio (local) com o tri�ngulo atrav�s do algoritmo de 10 
linhas visto em sala (lab 1) no dia 5/11. 


#-----------------------------------------##------------------------------------------#
A3 - #

#-----------------------------------------##------------------------------------------#
A4 - Implementamos na classe RayTracer o m�todo RayTracer::shade(), o qual � o 
respons�vel por tonalizar o ponto de intersec��o. A tonaliza��o de um ponto de 
intersec��o � realizada em dois passos principais: o c�lculo da ilumina��o direta 
atrav�s, dos raios de luz; c�lculo da ilumina��o indireta atrav�s do raio de reflex�o.

Para o c�lculo da ilumina��o direta, estabelecemos na cena uma estrutura chamada 
_lights*, a qual armazena o endere�o das luzes contidas dentre os objetos de cena da 
cena. Assim, para a ilumina��o direta, criamos um raio de luz para cada luz com origem
no ponto de intersec��o e direcionado para a luz, caso o raio de luz intercepte algum
outro objeto da hierarquia, ent�o este � um raio de sombra (cor preta), caso contr�rio
calculamos o efeito luminoso da luz no ponto atrav�s do modelo de ilumina��o de phong.

Para o c�lculo da ilumina��o direta, criamos um raio de reflex�o com origem no ponto
de intersec��o e com a dire��o de reflex�o do raio original, tal raio tem sua 
contribui��o para a ilumina��o do ponto medida pela cor da intersec��o deste com outro
ponto da hierarquia e assim recursivamente. Utilizamos para esta parte o algoritmo
explicado em sala (lab 3).

A ilumina��o de um ponto se d� ent�o pela soma da ilumina��o direta com a ilumina��o
indireta.

#-----------------------------------------##------------------------------------------#
A5 - Criamos duas cenas para testar a implementa��o do tra�ado de raios. Para se 
selecionar a cena desejada criamos um menu na janela de exibi��o chamado "Scenes", 
este por sua vez cont�m duas op��es selecion�veis, "Scene 1" e "Scene 2", a altera��o
da cena corrente no menu causa a invoca��o do m�todo P4::switchScenes(), o qual 
atualiza o atributo _currentScene de P4. Criamos tr�s materiais: Hard, Soft e Curly e 
os utilizamos em nossos objetos da primeira cena, criamos tamb�m um material default, 
o qual � utilizado caso um primitivo n�o possua outra especifica��o de material. A 
seguir damos uma breve explica��o das cenas:

Scene 1: A Scene 1 cont�m 7 objetos:
 - 1 luz pontual, posicionada pr�xima aos objetos, um pouco mais acima.
 - 4 boxes, uma grande para melhor se visualizar os outros objetos refletidos nela, 
	uma m�dia que far� sombra nas outras e duas pequenas dispostas de maneira a 
	uma ser escondida da outra
 - 1 esfera, no centro da cena, para interferir nas reflex�es e sombras, criando assim
	um bom efeito para se visualizar o funcionamento do tra�ado de raios. Esta 
	esfera tem um coeficiente specular definido como verde, assim fica mais f�cil
	distinguir seus efeitos na reflex�o.
 - 1 c�mera, chamada Camera 0, cuja vista ser� renderizada com o tra�ado de raios.

Dispomos as boxes de maneira a ficar claro o come�o e fim de suas sombrasque suas 
sombras. Deixamos a cor ambiente da cena um pouco mais escura, para que o efeito da 
luz fosse melhor percebido.

A dire��o default da c�mera Scene Editor est� contr�ria � dire��o da Camera 0, uma boa
maneira de se observar cena � caminhar um pouco com a Scene Editor atrav�s das teclas
de atalho, selecionar Camera 0 na hierarquia e visualizar sua posi��o, ent�o 
selecionar outro objeto que n�o seja uma c�mera para que a renderiza��o funcione 
corretamente e ent�o invocar o tra�ado de raios atrav�s do modo de vista de 
renderiza��o.

Scene 2: A Scene 2 cont�m 6 objetos:
 - 2 luzes, uma pontual pr�xima aos objetos de cena para que seu efeito seja maior e
   e outra direcional, cuja dire��o � a mesma de eixo z.
 - 1 esfera m�dia, 
 - 2 boxes, 1 grande para se analisar a reflex�o da esfera nela e outra pequena ao lado
   da esfera para se analisar a reflex�o no n�vel 2.
 - 1 c�mera, chamada Camera 0, esta se encontra praticamente na mesma posi��o padr�o
   da Scene Editor.

Nosso objeto com esta cena foi de posicionar a c�mera voltada � parte de menos 
ilumina��o dos objetos e assim, ao renderizar, mostrar o funcionamento do algoritmo 
de c�lculo dos raios secund�rios. Observamos nesta cena que a reflex�o de n�vel 2 
partindo da esfera tem um funcionamento peculiar, ela parece ser realizada em uma 
dire��o de reflex�o calculada � partir da normal invertida dos tri�ngulos da malha
das esferas.


* Obs.: 
 - Notamos um problema em rela��o a reflex�o das esferas,
   estas parecem calcular a ilumina��o secund�ria utilizando a normal invertida 
   nos c�lculos, este problema n�o foi solucionado.

 - Se o tra�ado de raios for invocado enquanto uma c�mera da cena est� como objeto 
   corrente, a cena n�o ser� renderizada devidamente.
#-----------------------------------------##------------------------------------------#

#-----------------------------------------##------------------------------------------#

Observa��es:

 - No m�todo P4::hieararchyWindow implementamos a remo��o (sobrecarregada) de uma luz 
   da cena, a qual � invocada ao se deletar um objeto da cena que contenha uma luz.

 - Para a cria��o de duas cenas, objeto do A5, reformamos um pouco da estrutura da 
   classe P4, criamos o atributo Scene::_currentCamera, o qual substitui o atributo 
   Camera::_current, pois agora cada uma das cenas ter� sua pr�pria c�mera corrente e 
   um atributo est�tico compartilhado entre cenas n�o funcionaria.

 - Tivemos um constante problema com a reflex�o das esferas, colocamos uma esfera em 
   nossa cena, a qual n�o teve a luz de reflex�o perfeita, por�m deu-se para notar uma 
   reflex�o pr�xima da ideal.