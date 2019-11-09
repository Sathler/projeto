import sqlite3
import os

from . import helpers

USUARIOS_TIPOS = ['comum', 'admin']

class AnimeShell:

    def __init__(self, prompt='>> '):
        self.prompt = prompt
        self.comandos = {
            'sair':self.sair,
            'ajuda':self.ajuda,
            'adiciona-anime':self.adiciona_anime,
            'remove-anime':self.remove_anime,
            'cria-anime':self.cria_anime,
            'deleta-anime':self.deleta_anime,
            'filtra-animes':self.filtra_animes,
            'cria-estudio':self.cria_estudio,
            'cria-genero':self.cria_genero,
            'deleta-usuario':self.deleta_usuario,
            'promove-usuario':self.promove_usuario,
            'rebaixa-usuario':self.rebaixa_usuario,
            'login': self.efetuar_login,
            'logout': self.efetuar_logout,
            'cadastro': self.efetuar_cadastro
        }
        self.comandos_ajuda = {
            'sair': 
                'Sai do programa.'
                '\nUso: ">> sair"',
            'ajuda':
                'Mostra todos os comandos disponíveis, se não receber nenhum '
                'argumento, ou a mensagem de ajuda de um comando, caso o '
                'receba.'
                '\nUso: ">> ajuda (comando)"',
            'adiciona-anime': 
                'Adiciona um anime para a lista do usuário atual.'
                '\nUso: ">> adiciona-anime id-do-anime"',
            'remove-anime': 
                'Remove um anime da lista do usuário atual.'
                '\nUso: ">> remove-anime id-do-anime"',
            'cria-anime': 
                'Cria um anime no banco de dados.'
                '\nUso: ">> cria-anime"',
            'deleta-anime': 
                'Deleta um anime do banco de dados.'
                '\nUso: ">> deleta-anime id-do-anime"',
            'filtra-animes':
                'Se não receber nenhum argumento, imprime todos os animes do '
                'banco de dados.'
                '\nCaso receba uma ou mais palavras-chave, as usa como filtro.'
                '\nOs IDs de cada anime são mostrados antes de seus nomes.'
                '\nUse o comando "-u" para ver somente os do usuário '
                '(só funciona para usuários logados).'
                '\nUso: ">> filtra-animes (-u)"',
            'cria-estudio':
                'Cria um estúdio de animação no banco de dados.'
                '\nUso: ">> cria-estudio"',
            'cria-genero':
                'Cria um gênero de anime no banco de dados.'
                '\nUso: ">> cria-genero (genero_sem_espaços_em_branco)"',
            'deleta-usuario':
                'Deleta um usuário do banco de dados'
                '\nUso: ">> deleta-usuario apelido-usuario"',
            'promove-usuario':
                'Promove um usuário para administrador.'
                '\nUso: ">> promove-usuario apelido-usuario"',
            'rebaixa-usuario':
                'Rebaixa um usuário para usuário comum.'
                '\nUso: ">> rebaixa-usuario apelido-usuario"',
            'login':
                'Efetuar login como um usuário'
                '\nUso: ">> login"',
            'logout':
                'Efetuar logout'
                '\nUso: ">> logout"',
            'cadastro':
                'Cadastrar-se no sistema'
                '\nUso: ">> cadastro"'
        }
        self.comandos_permissoes = {
            'sair':['comum', 'admin', 'deslogado'],
            'ajuda':['comum', 'admin', 'deslogado'],
            'adiciona-anime': ['comum', 'admin'],
            'remove-anime': ['comum', 'admin'],
            'cria-anime': ['admin'],
            'deleta-anime': ['admin'],
            'filtra-animes': ['comum', 'admin', 'deslogado'],
            'cria-estudio': ['admin'],
            'cria-genero': ['admin'],
            'deleta-usuario': ['comum', 'admin'],
            'promove-usuario': ['admin'],
            'rebaixa-usuario': ['admin'],
            'login':  ['deslogado'],
            'logout':  ['comum', 'admin'],
            'cadastro':  ['deslogado']
        }

        self.flag_sair = None
        self.usuario_atual_nome = ''
        self.usuario_atual_id = ''
        self.usuario_atual_tipo = 'deslogado'
        # Levanta RuntimeError
        self.banco_de_dados = ListaAnimes()
    
    def executar(self):
        self.flag_sair = False
        print()
        print('Para ver a lista de comandos, digite "ajuda".', end='\n\n')
        while not self.flag_sair:

            linha = input(self.prompt)
            comando, atributos = self._separa_comando_atributos(linha)
            if comando == '':
                continue

            if comando not in self.comandos:
                print('Comando inválido. Insira "ajuda" para mais informações.')
            elif self.usuario_atual_tipo not in self.comandos_permissoes[comando]:
                print('Você não tem permissão para utilizar este comando.')
            else:
                try:
                    self.comandos[comando](*atributos)
                except TypeError:
                    print('Número de argumentos inválido para {}.'.format(comando))
            print()
        
        self.banco_de_dados.fechar()

        
    def sair(self):
        self.flag_sair = True

    def ajuda(self, comando=None):
        print()
        if comando is None:
            pass
        elif comando in self.comandos:
            print(self.comandos_ajuda[comando])
            print('Permissões: ' + ', '.join(self.comandos_permissoes[comando]) + '.')
            print()
            return
        else:
            print('{} não é um comando válido.'.format(comando))

        print('Comandos disponíveis: ')
        for c in self.comandos.keys():
            print(' - {}'.format(c))
        print()
        print(self.comandos_ajuda['ajuda'])
        print()
                

    def adiciona_anime(self, id_anime):

        try:
            resultados = self.banco_de_dados.select(
                'anime',
                'cod_anime',
                'cod_anime = ?',
                [id_anime]
            )
        except Exception as e:
            self._print_erro_bd(e)
            return
        
        if len(resultados) == 0:
            print('ID não equivale a nenhum item do banco de dados.')
            return

        try:
            self.banco_de_dados.insert(
                'assiste',
                {'fk_anime_cod_anime': id_anime,
                 'fk_usuario_cod_usuario': self.usuario_atual_id}
            )
        except sqlite3.IntegrityError:
            print('Item já está em sua lista.')
            return
        except Exception as e:
            self._print_erro_bd(str(e))
            return
        
        print('Anime adicionado com sucesso.')

    
    def remove_anime(self, id_anime):
        try:
            linhas_afetadas = self.banco_de_dados.delete(
                'assiste',
                'fk_anime_cod_anime = ? AND fk_usuario_cod_usuario = ?',
                [id_anime, self.usuario_atual_id]
            )
        except Exception as e:
            print(str(e))
            return

        if linhas_afetadas == 0:
            print('ID não equivale a nenhum item do banco de dados.')
        else:
            print('Anime excluído com sucesso.')

    
    def filtra_animes(self, meus=''):
        """Filtra animes de acordo com as informações passadas."""
        termo_busca = input('Query [Padrão: todos]: ')
        campos_selecionar = [
            'cod_anime',
            'nome'
        ]
        campos_where = [
            'cod_anime',
            'nome',
            'score',
            'visualizacoes',
        ]

        tabela = 'anime'
        if meus == '-u' and self.usuario_atual_tipo != 'deslogado':
            tabela += (
            ' INNER JOIN (SELECT fk_anime_cod_anime FROM assiste WHERE fk_usuario_cod_usuario = {}) t2'
            ' ON t2.fk_anime_cod_anime = anime.cod_anime').format(self.usuario_atual_id)
        campos = ', '.join(campos_selecionar)
        where = ' OR '.join(['UPPER({}) LIKE UPPER(?)'.format(campo) for campo in campos_where])

        sql = 'SELECT DISTINCT {campos} FROM {tabela} WHERE {condicoes}'.format(
            campos=campos, tabela=tabela, condicoes=where
        )
        
        try:
            _, linhas = self.banco_de_dados.comando_geral(
                sql,
                [r'%{}%'.format(termo_busca)]*len(campos_where)
            )
        except Exception as e:
            self._print_erro_bd(e)
            return
        
        print('ID'+' '*6 + '| ' + 'Nome')
        print('--'+'-'*6 + '| ' + '-'*60)
        for (id_anime, nome) in linhas:
            print('{}| {}'.format(str(id_anime).ljust(8), nome))
    

    def cria_anime(self):
        nome = input("Nome do anime: ")
        if nome == '':
            print("Nome do anime não pode estar em branco.")
            return
        score = 0
        sinopse = helpers.input_multiplas_linhas("Insira uma sinopse.")
        visualizacoes = 0
        data_de_lancamento = input("Data de lançamento (YYYY-MM-DD): ")
        if not helpers.eh_data_valida(data_de_lancamento):
            print("Data deve estar no formato YYYY-MM-DD.")
            return
        data_de_encerramento = input("Data de encerramento (YYYY-MM-DD): ")
        if not helpers.eh_data_valida(data_de_encerramento):
            print("Data deve estar no formato YYYY-MM-DD.")
            return
        if data_de_lancamento > data_de_encerramento:
            print("Data de lançamento não pode ser maior que data de encerramento.")
            return
        status_anime = input("Status do anime ('ainda_nao_comecou', 'em_exibicao', 'terminado', 'hiato'):\n>> ")
        if status_anime not in ['ainda_nao_comecou', 'em_exibicao', 'terminado', 'hiato']:
            print("Status inválido.")
            return
        numero_de_episodios = input("Número de episódios: ")
        try:
            numero_de_episodios = int(numero_de_episodios)
            if numero_de_episodios < 0:
                raise ValueError
        except ValueError:
            print("Número de episódios deve ser um número inteiro maior ou igual a zero.")
            return
        
        try:
            self.banco_de_dados.insert(
                'anime',
                {
                    'nome': nome,
                    'score': score,
                    'sinopse': sinopse,
                    'visualizacoes': visualizacoes,
                    'data_de_lancamento': data_de_lancamento,
                    'data_de_encerramento': data_de_encerramento,
                    'status_anime': status_anime,
                    'numero_de_episodios': numero_de_episodios
                }
            )
        except sqlite3.IntegrityError:
            print("Já existe um anime com esse mesmo nome.")
            return
        except Exception as e:
            self._print_erro_bd(e)
            return
        
    
    def deleta_anime(self, id_anime):
        try:
            linhas_alteradas = self.banco_de_dados.delete(
                'anime',
                'cod_anime = ?',
                [id_anime]
            )
        except Exception as e:
            self._print_erro_bd(e)
            return

        if linhas_alteradas > 0:
            print("Anime de código '{}' removido com sucesso.".format(id_anime))
        else:
            print("Anime de código '{}' não existe.".format(id_anime))
    
    def cria_estudio(self):
        estudio = input('Nome do estúdio: ')
        if estudio == '':
            print("Entrada para estúdio não pode estar vazia.")
            return

        try:
            self.banco_de_dados.insert(
                'estudio',
                {'nome': estudio}
            )
        except sqlite3.IntegrityError:
            print("Estúdio '{}' já existe.".format(estudio))
            return
        except Exception as e:
            self._print_erro_bd(e)
            return

        print("Estúdio '{}' criado com sucesso.".format(estudio))

    
    def cria_genero(self, genero=None):
        if genero is None:
            genero = input('Gênero: ')
        if genero == '':
            print("Entrada para gênero não pode estar vazia.")
            return
        genero = self._gera_genero_seguro(genero)
        
        try:
            self.banco_de_dados.insert(
                'genero',
                {'nome': genero}
            )
        except sqlite3.IntegrityError:
            print("Já existe gênero de valor '{}'.".format(genero))
            return
        except Exception as e:
            self._print_erro_bd(e)
            return
        
        print("Gênero '{}' adicionado com sucesso.".format(genero))
    
    
    def deleta_usuario(self, apelido):
        
        if apelido == self.usuario_atual_nome:
            print('Você não pode apagar a sí mesmo.')
            return
        
        try:
            linhas_afetadas = self.banco_de_dados.delete(
                'usuario',
                'apelido = ?',
                [apelido]
            )
        except Exception as e:
            self._print_erro_bd(e)
            return
        
        if linhas_afetadas > 0:
            print("Usuário '{}' apagado com sucesso.".format(apelido))
        else:
            print("Usuário '{}' não existe.".format(apelido))
            
    
    def promove_usuario(self, apelido):
        self._muda_tipo_usuario(apelido, 'admin')
    
    def rebaixa_usuario(self, apelido):
        self._muda_tipo_usuario(apelido, 'comum')


    
    def efetuar_login(self):
        try:
            usuario, senha = self._pede_login_senha()
        except Exception as e:
            print(str(e))
            return

        try:
            linhas = self.banco_de_dados.select(
                'usuario',
                ['cod_usuario', 'senha', 'tipo_usuario'],
                'apelido = ?',
                [usuario]
            )
        except Exception as e:
            self._print_erro_bd(e)
            return

        if len(linhas) == 0:
            print("Usuário '{}' não encontrado.".format(usuario))
            return
        
        if linhas[0][1] != senha:
            print("Senha incorreta.")
            return
        
        self.usuario_atual_nome = usuario
        self.usuario_atual_id = linhas[0][0]
        self.usuario_atual_tipo = linhas[0][2]

        print()
        print('Bem-vindo(a), {}.'.format(usuario))
        

    
    def efetuar_logout(self):
        usuario = self.usuario_atual_nome
        self.usuario_atual_nome = self.usuario_atual_id = ''
        self.usuario_atual_tipo = 'deslogado'
        print()
        print("Tchau, {}".format(usuario))
    
    def efetuar_cadastro(self):
        try:
            usuario, senha = self._pede_login_senha(
                'Novo usuário: ', 'Nova senha: '
            )
        except Exception as e:
            print(str(e))
            return
        
        try:
            self.banco_de_dados.insert(
                'usuario',
                {'apelido': usuario, 'senha': senha,
                 'idade': None, 'tipo_usuario': 'comum'}
            )
        except sqlite3.IntegrityError:
            print("Usuário '{}' já existe.")
            return
        except Exception as e:
            self._print_erro_bd(e)
            return
        
        print()
        print('Usuário criado com sucesso.'
              '\nPor favor, efetue login para entrar em sua conta.')


    def _muda_tipo_usuario(self, apelido, tipo):

        if apelido == self.usuario_atual_nome:
            print('Você não pode promover a sí mesmo.')
            return
        if tipo not in ['comum', 'admin']:
            raise ValueError("Tipo de usuário inválido.")

        try:
            linhas_afetadas = self.banco_de_dados.update(
                'usuario',
                {'tipo_usuario':tipo},
                'apelido = ?',
                [apelido]
            )
        except Exception as e:
            self._print_erro_bd(e)
            return

        if linhas_afetadas > 0:
            print("Usuário '{}' agora é '{}'.".format(apelido, tipo))
        else:
            print("Usuário '{}' não existe.".format(apelido))


    def _separa_comando_atributos(self, linha):
        """Separa uma string em comando e atributo."""

        elementos = linha.split()
        if len(elementos) == 0:
            comando, atributos = '', []
        elif len(elementos) == 1:
            comando, atributos = elementos[0], []
        else:
            comando, atributos = elementos[0], elementos[1:]
        
        comando = comando.lower()
        return comando, atributos
    
    def _pede_login_senha(
        self, msg_login='Nome de usuário: ', msg_senha='Senha: '):
        #
        """Pede login e senha do usuário"""

        login = input(msg_login)
        import re
        login_seguro = re.sub(r'\s+', r'_', login)
        login_seguro = re.sub(r'[^A-Za-z0-9_\-]', '', login_seguro)
        if login != login_seguro or login == '':
            raise RuntimeError(
                'Valor de login deve ter somente letras maiúsculas e minúsculas'
                ' sem acentos, números, underscores e hífens.')
        from getpass import getpass
        senha = getpass(msg_senha)
        if senha == '':
            raise RuntimeError('Senha não pode estar em branco.')
        return login, senha
    
    def _print_erro_bd(self, msg_erro):
        print("\nErro de banco de dados:\n"+str(msg_erro))
        
    def _gera_genero_seguro(self, nome):
        """Converte o gênero para o formato adequado.

        Converte todas as letras especiais para equivalentes em inglês
        e maiúsculas para minúsculas

        Só ficam a-z, 0-9, _'.
        """
        nome = nome.lower()
        ficha_troca = str.maketrans(
            "aeiouáéíóúâêîôûãõàèìòùñçäëïöü-",
            "aeiouaeiouaeiouaoaeiouncaeiou_"
        )
        nome = nome.translate(ficha_troca)
        import re
        nome = re.sub(r'\s+', r'_', nome)
        nome = re.sub(r'[^A-Za-z0-9_]', '', nome)
        
        return nome




import __main__
ROOT_DIR = os.path.abspath(os.path.dirname(__main__.__file__))
DIR_SCRIPTS = os.path.join(ROOT_DIR, 'scripts')
PATH_SCRIPT_GERADOR = os.path.join(DIR_SCRIPTS, 'gerador.sql')

class ListaAnimes:

    def __init__(self):

        self.bd_dir = os.path.join(ROOT_DIR, 'bd')
        self.bd_path = os.path.join(self.bd_dir, 'animes.db')

        if not os.path.isdir(self.bd_dir):
            try:
                os.mkdir(self.bd_dir)
            except OSError:
                raise OSError('Não foi possível criar pasta para o banco de dados.')
    
        # Levanta RuntimeError.
        self.conn = self._cria_tabela_se_nao_existir()
    
    def fechar(self):
        self.conn.close()
    

    def insert(self, tabela, campos_valores):
        """Insere valores em campos de uma tabela.

        Parameters
        ------------
        tabela : str
            Nome da tabela no banco de dados.
        campos_valores : dict 
            campos da tabela a terem valores adicionados.
            No formato {'campo':'valor'}.
        
        Returns
        --------
        int
            Linhas alteradas.
        
        Raises
        -------
        ValueError
            Se 'campos_valores' estiver vazio.
        sqlite3.Error
            Se não for possível inserir a linha na tabela.
        """
        if len(campos_valores) == 0:
            raise ValueError("'campos_valores' não pode estar vazio.")
        campos = ', '.join(campos_valores.keys())
        valores = campos_valores.values()
        placeholders_valores = ', '.join(['?']*len(valores))

        sql = 'INSERT INTO {}({}) VALUES({})' \
              .format(tabela, campos, placeholders_valores)

        linhas_alteradas, _ = self.comando_geral(sql, valores) 
        return linhas_alteradas
        


    def update(self, tabela, campos_valores, condicoes_str, condicoes_valores):
        """
        Atualiza valores em campos se todas as condições forem
        satisfeitas.

        Parameters
        ------------
        tabela : str
            Nome da tabela no banco de dados.
        campos_valores : dict
            campos da tabela a terem valores adicionados.
            No formato {'campo':'valor'}.
        condicoes_str : str
            Comandos em string no formato SQL com '?' no lugar dos
            valores. Ex.: 'id = ? AND episodios > ?'.
        condicoes_valores : list
            Lista de valors que substituirão as '?' na string de
            'condicoes'. Ex.: [1, 39].
        
        Returns
        --------
        int
            Linhas afetadas.
        
        Raises
        ---------
        ValueError
            Se 'campos_valores' estiver vazio.
        sqlite3.Error
            Se não for possível utilizar o comando de atualização na
            tabela.
        """
        if len(campos_valores) == 0:
            raise ValueError("'campos_valores' não pode estar vazio.")
        campos_set = ', '.join( [c+' = ?' for c in campos_valores.keys()] ) 

        sql = 'UPDATE {} SET {} WHERE {}' \
              .format(tabela, campos_set, condicoes_str)
        
        valores_finais = list(campos_valores.values()) + condicoes_valores
    
        linhas_alteradas, _ = self.comando_geral(sql, valores_finais) 
        return linhas_alteradas


    def delete(self, tabela, condicoes_str, condicoes_valores):
        """
        Deleta linhas de uma tabela se todas as condições forem
        satisfeitas.

        Parameters
        ------------
        tabela : str
            Nome da tabela no banco de dados.
        condicoes_str : str
            Comandos em string no formato SQL com '?' no lugar dos
            valores. Ex.: 'id = ? AND episodios > ?'.
        condicoes_valores : list
            Lista de valors que substituirão as '?' na string de
            'condicoes'. Ex.: [1, 39].
        
        Returns
        --------
        int
            Linhas afetadas.
        
        Raises
        ---------
        sqlite3.Error
            Se não for possível utilizar o comando de deleção na tabela.
        """

        sql = 'DELETE FROM {} WHERE {}' \
              .format(tabela, condicoes_str)
        
        valores_finais = condicoes_valores

        linhas_alteradas, _ = self.comando_geral(sql, valores_finais)    
        return linhas_alteradas

    def select(self, tabela, colunas, condicoes_str, condicoes_valores):
        """
        Deleta linhas de uma tabela se todas as condições forem
        satisfeitas.

        Parameters
        ------------
        tabela : str
            Nome da tabela no banco de dados.
        colunas : str or list of str
            Coluna(s) a ser(em) escolhida(s).
            Aceita-se uma string uma lista de strings representando
            os campos.
        condicoes : str
            Comandos em string no formato SQL com '?' no lugar dos
            valores. Ex.: 'id = ? AND episodios > ?'.
        condicoes_valores : list
            Lista de valors que substituirão as '?' na string de
            'condicoes'. Ex.: [1, 39].
        
        Returns
        --------
        list of list
            Matriz com as linhas e colunas da tabela.
        
        Raises
        ---------
        sqlite3.Error
            Se não for possível utilizar o comando de deleção na tabela.
        """
        if isinstance(colunas, str):
            campos_select = colunas
        else:
            campos_select = ', '.join( colunas ) 

        sql = 'SELECT {} FROM {} WHERE {}' \
              .format(campos_select, tabela, condicoes_str)
        
        valores_finais = condicoes_valores
    
        _, resultados = self.comando_geral(sql, valores_finais)
        return resultados
    
    def comando_geral(self, comando, valores):
        """Executa um comando SQL qualquer.

        Parameters
        -----------
        comando : str
            Um comando SQL qualquer, com todos os valores substituídos
            por '?'. Ex.: 'INSERT INTO forms VALUES (?, ?)
        valores : list
            Uma lista com os valores que substituirão as '?' no
            'comando'. Ex.: ['nome', 'email']
        
        Returns
        --------
        int
            Linhas afetadas. Se não for um comando que afete listas,
            será -1.
        list of list or list
            Matriz com as linhas retornadas ou uma lista vazia.
        """
        cursor = self.conn.cursor()
        # Levantam sqlite3.Error
        cursor.execute(comando, tuple(valores))
        self.conn.commit()
        
        return cursor.rowcount, cursor.fetchall()



    def _abrir_bd(self):
    
        try:
            conn = sqlite3.connect(self.bd_path)
        except:
            raise RuntimeError('Não foi possível abrir o banco de dados.')
        cursor = conn.cursor()
        # Força uso de chaves estrangeiras.
        cursor.execute('PRAGMA foreign_keys = ON;')
        return conn
        
    
    def _cria_tabela_se_nao_existir(self):
        #Levanta RuntimeError
        bd_ja_existia = os.path.isfile(self.bd_path)

        conn = self._abrir_bd()

        if not bd_ja_existia:
            try:
                with open(PATH_SCRIPT_GERADOR, 'r') as arq_gerador:
                    script_gerador = arq_gerador.read()
            except OSError:
                raise OSError('Não foi possível abrir script gerador: "{}"'
                              .format(PATH_SCRIPT_GERADOR))
            cursor = conn.cursor()
            cursor.executescript(script_gerador)
            conn.commit()
        
        return conn