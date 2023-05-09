from flask import Flask, request, Response
import json
app = Flask(__name__)

class TodoModel:
    def __init__(self, content, status) -> None:
        self.content = content
        self.status = status

todoList = []

@app.get('/')
def getAllTodos() :
    global todoList
    todoList = []
    with open('todo.json') as f:
        data = json.load(f)
        for todo in data:
            todoList.append(TodoModel(todo['content'], todo['status']).__dict__)
    data = { "data" : todoList }
    resp = Response(response=json.dumps(data), status=200, content_type="application/json")
    return resp

@app.post('/')
def postTodo() :
    global todoList
    content_type = request.headers.get('Content-Type')
    if(content_type == 'application/json'):
        body = request.get_json()
        todoList.append(TodoModel(body['content'], body['status']).__dict__)
        
        f = open('todo.json', 'w')
        jsonString = json.dumps(todoList, indent=4)
        f.write(jsonString)
        f.close()
    resp = Response(status=200)
    return resp
    
    
    

if __name__ == "__main__" :
    app.run(port=3000)