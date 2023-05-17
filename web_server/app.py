from flask import Flask, Response, request
import json
app = Flask(__name__)


class FILE_MODE :
    READ = 0
    WRITE = 1

class TodoItem :
    def __init__(self, id, content, status):
        self.id = id
        self.content = content
        self.status = status

class TodoItemResponse :
    def __init__(self, list_todo):
        self.data = list_todo

todoList = []
@app.get('/')
def getAllTodos():
    handle_file(FILE_MODE.READ)
    response = { "data" : todoList }
    log()
    resp = Response(
        response=json.dumps(response), status=200,  mimetype="application/json"
    )
    return resp
    
@app.post('/')
def postTodo():
    body = request.get_json()
    todoList.append(TodoItem(int(body['id']), body['content'], body['status']).__dict__)
    log()
    handle_file(FILE_MODE.WRITE)
    resp = Response(
        status=200
    )
    return resp

@app.put('/')
def putTodo():
    log()
    id = request.args['id']
    id = int(id)
    body = request.get_json()
    for item in todoList:
        if item['id'] == id:
            item['status'] = body['status']
            if len(body['content']) != 0 or len(body['content']) == item['content']: 
                item['content'] = body['content']
    handle_file(FILE_MODE.WRITE)
    resp = Response(
        status=200
    )
    return resp

@app.delete('/')
def deleteTodo():
    id = request.args['id']
    id = int(id)
    for item in todoList :
        if item['id'] == id:
            todoList.remove(item)
    handle_file(FILE_MODE.WRITE)
    resp = Response(
        status=200
    )
    return resp
    
    
    

def log():
    print("Todo List : {}".format(todoList))
def handle_file(mode = FILE_MODE.READ) :
    if mode == FILE_MODE.READ:
        todoList.clear()
        f = open('todo.json')
        data = json.load(f)
        f.close()
        for item in data:
            todoList.append(TodoItem(item['id'],item['content'],item['status']).__dict__)
    else :
        f = open('todo.json', 'w')
        data = json.dumps(todoList, indent=4)
        f.write(data)
        f.close()


if __name__ == '__main__':
    if len(todoList) == 0:
        f = open('todo.json')
        data = json.load(f)
        f.close()
        for item in data:
            todoList.append(TodoItem(item['id'],item['content'],item['status']).__dict__)
    app.run(host='0.0.0.0', port=3000, debug=True)