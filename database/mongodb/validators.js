db = db.getSiblingDB('smartlibrary');

db.createCollection('eventos', {
  validator: {
    $jsonSchema: {
      bsonType: 'object',
      required: ['tipo', 'dataHora', 'origem'],
      properties: {
        tipo: { bsonType: 'string' },
        dataHora: { bsonType: 'date' },
        usuarioId: { bsonType: ['int', 'long', 'null'] },
        emprestimoId: { bsonType: ['int', 'long', 'null'] },
        origem: { enum: ['BALCAO', 'SELF_CHECKOUT', 'SISTEMA'] },
        terminal: {
          bsonType: ['object', 'null'],
          properties: {
            id: { bsonType: ['int', 'long'] },
            codigo: { bsonType: 'string' }
          }
        },
        exemplar: {
          bsonType: ['object', 'null'],
          properties: {
            id: { bsonType: ['int', 'long'] },
            codigoBarras: { bsonType: 'string' }
          }
        }
      }
    }
  }
});

db.createCollection('logs', {
  validator: {
    $jsonSchema: {
      bsonType: 'object',
      required: ['nivel', 'dataHora', 'mensagem'],
      properties: {
        nivel: { enum: ['DEBUG', 'INFO', 'WARN', 'ERROR'] },
        dataHora: { bsonType: 'date' },
        componente: { bsonType: 'string' },
        terminalCodigo: { bsonType: ['string', 'null'] },
        mensagem: { bsonType: 'string' },
        detalhes: { bsonType: ['object', 'array', 'string', 'null'] }
      }
    }
  }
});

db.createCollection('auditoria', {
  validator: {
    $jsonSchema: {
      bsonType: 'object',
      required: ['entidade', 'entidadeId', 'acao', 'dataHora'],
      properties: {
        entidade: { bsonType: 'string' },
        entidadeId: { bsonType: ['int', 'long', 'string'] },
        acao: { enum: ['CRIACAO', 'ALTERACAO', 'EXCLUSAO', 'ATIVACAO', 'DESATIVACAO'] },
        usuarioId: { bsonType: ['int', 'long', 'null'] },
        dataHora: { bsonType: 'date' },
        antes: { bsonType: ['object', 'null'] },
        depois: { bsonType: ['object', 'null'] }
      }
    }
  }
});
