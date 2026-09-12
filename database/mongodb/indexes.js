const databaseName = process.env.MONGODB_DATABASE || 'smartlibrary';
db = db.getSiblingDB(databaseName);

db.eventos.createIndex({ tipo: 1, dataHora: -1 });
db.eventos.createIndex({ 'terminal.codigo': 1, dataHora: -1 });
db.eventos.createIndex({ usuarioId: 1, dataHora: -1 });
db.eventos.createIndex({ origem: 1, dataHora: -1 });

db.logs.createIndex({ nivel: 1, dataHora: -1 });
db.logs.createIndex({ terminalCodigo: 1, dataHora: -1 });
db.logs.createIndex({ componente: 1, dataHora: -1 });

db.auditoria.createIndex({ entidade: 1, entidadeId: 1, dataHora: -1 });
db.auditoria.createIndex({ usuarioId: 1, dataHora: -1 });
