from flask import Flask, request

app = Flask(__name__)

@app.route('/upload', methods=['POST'])
def upload_file():
    print('Running upload...')
    print(request.headers)
    print(request)
    file = request.files['file']
    file.save(file.filename)
    print('File saved successfully')
    return 'File saved successfully'

if __name__ == '__main__':
    app.run(debug=True)