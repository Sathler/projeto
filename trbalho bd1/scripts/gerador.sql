/* Lógico_1: */

/*
 * No sqlite:
 * 1. Tabelas têm, por padrão, uma chave primária inteira (rowid) que
 *    recebe um inteiro que não esteja em uso no momento.
 * 2. Se a chave primária explícita for de tipo INTEGER, ele
 *    se torna um alias para rowid.
 * 3. NUMERIC e INTEGER são o mesmo tipo, mas o uso de INTEGER é o que faz
 *    a chave primária um alias de rowid (NUMERIC não faz).
 * 3. AUTOINCREMENT poderia ser usado para forçar rowid a usar um inteiro
 *    que nunca foi usado, mas não é recomendado por gastar mais recursos.
 * 4. Não se pode usar 'ALTER TABLE' para adicionar chaves extrangeiras.
 */

CREATE TABLE estudio (
    cod_estudio INTEGER PRIMARY KEY,
    nome VARCHAR UNIQUE,
    CHECK (length(nome) > 0)
);

CREATE TABLE anime (
    cod_anime INTEGER PRIMARY KEY,
    nome VARCHAR UNIQUE,
    score NUMERIC,
    sinopse VARCHAR,
    visualizacoes NUMERIC,
    data_de_lancamento DATE,
    data_de_encerramento DATE,
    status_anime VARCHAR,
    numero_de_episodios NUMERIC,
    CHECK (length(nome) > 0),
    CHECK (score >= 0 AND score <= 10),
    CHECK (visualizacoes >= 0),
    CHECK (numero_de_episodios >= 0), /* Caso não tenha começado*/
    CHECK (data_de_lancamento <= data_de_encerramento),
    CHECK (status_anime in ('ainda_nao_comecou', 'em_exibicao', 'terminado', 'hiato'))
);

CREATE TABLE genero (
    nome VARCHAR PRIMARY KEY,
    CHECK (length(nome) > 0)
);

CREATE TABLE define (
    fk_genero_nome VARCHAR,
    fk_anime_cod_anime INTEGER,
    FOREIGN KEY (fk_genero_nome)
        REFERENCES genero (nome)
        ON DELETE NO ACTION,
    FOREIGN KEY (fk_anime_cod_anime)
        REFERENCES anime (cod_anime)
        ON DELETE CASCADE,
    PRIMARY KEY (fk_genero_nome, fk_anime_cod_anime)    
);

CREATE TABLE funcionario (
    cod_func INTEGER PRIMARY KEY,
    nome VARCHAR,
    CHECK (length(nome) > 0)
);

CREATE TABLE trabalha (
    fk_funcionario_cod_func INTEGER,
    fk_anime_cod_anime INTEGER,
    cargo VARCHAR,
    FOREIGN KEY (fk_funcionario_cod_func)
        REFERENCES funcionario (cod_func)
        ON DELETE RESTRICT,
    FOREIGN KEY (fk_anime_cod_anime)
        REFERENCES anime (cod_anime)
        ON DELETE RESTRICT,
    PRIMARY KEY (fk_funcionario_cod_func, fk_anime_cod_anime, cargo),
    CHECK (length(cargo) > 0)
);

CREATE TABLE usuario (
    cod_usuario INTEGER PRIMARY KEY,
    apelido VARCHAR UNIQUE,
    senha VARCHAR,
    idade NUMERIC,
    tipo_usuario VARCHAR,
    CHECK (length(apelido) > 0),
    CHECK (length(senha) > 0),
    CHECK (idade > 0),
    CHECK (tipo_usuario in ('admin', 'comum'))
);

CREATE TABLE generos_favoritos (
    fk_cod_usuario INTEGER NOT NULL,
    fk_genero VARCHAR NOT NULL,
    FOREIGN KEY (fk_cod_usuario)
        REFERENCES usuario (cod_usuario)
        ON DELETE CASCADE,
    FOREIGN KEY (fk_genero)
        REFERENCES genero (nome)
        ON DELETE NO ACTION,
    PRIMARY KEY (fk_cod_usuario, fk_genero)
);

CREATE TABLE cria (
    fk_estudio_cod_estudio INTEGER,
    fk_anime_cod_anime INTEGER,
    FOREIGN KEY (fk_estudio_cod_estudio)
        REFERENCES estudio (cod_estudio)
        ON DELETE NO ACTION,
    FOREIGN KEY (fk_anime_cod_anime)
        REFERENCES anime (cod_anime)
        ON DELETE NO ACTION,
    PRIMARY KEY (fk_estudio_cod_estudio, fk_anime_cod_anime)
);

CREATE TABLE assiste (
    fk_anime_cod_anime INTEGER,
    fk_usuario_cod_usuario INTEGER,
    numero_de_ep_assistidos NUMERIC,

    FOREIGN KEY (fk_anime_cod_anime)
        REFERENCES anime (cod_anime)
        ON DELETE RESTRICT,
    FOREIGN KEY (fk_usuario_cod_usuario)
        REFERENCES usuario (cod_usuario)
        ON DELETE CASCADE,
    PRIMARY KEY (fk_anime_cod_anime, fk_usuario_cod_usuario),
    CHECK (numero_de_ep_assistidos >= 0)
);

CREATE TRIGGER assite_episodios_trigger
BEFORE INSERT ON assiste
BEGIN
    SELECT RAISE(ABORT, 'Number of episodes watched cannot be greater than the number of episodes of a show.')
    FROM anime
    WHERE cod_anime = NEW.fk_anime_cod_anime
        AND numero_de_episodios < NEW.numero_de_ep_assistidos;
END;


INSERT INTO usuario (apelido, senha, tipo_usuario)
VALUES ('admin', 'admin', 'admin')