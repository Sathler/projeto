# Projeto
Projeto de Software Embarcado.

### Descrição

O projeto visa facilitar o gerenciamento de estoque de produtos. Será possível gerenciar o banco de dados de produtos.

Iremos usar um identificador e associá-lo ao produto para cadastrar no banco de dados.
Usaremos um arduino, com o leitor, conectado ao computador para ler o identificador.
O gerenciamento será com um outro arduino (móvel) o qual terá o leitor e um botão. Quando lermos o identificador de um produto, um LCD conectado ao arduino exibirá
as informações do produto. Caso o botão seja pressionado, o produto será excluído do banco de dados.


## Componentes


| **Nome do componente**                | **Quantidade** |
| -------------------------------- | ------------- |
| Arduino Mega 2560                                      | 1 |
| Arduino UNO                                            | 1 |
| Modulo Wireless NRF24l01 2.4ghz - Comunicação Sem Fio  |  para cada arduino |
| Arduino LCD Shield                                     | 1 |
| Kit Leitor RFID RC522                                  | para cada arduino |
| Botão táctil                                           | 1 |