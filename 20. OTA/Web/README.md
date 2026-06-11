# Flask Upload Template Split

## 구조

```text
templates/
└─ upload.html

static/
├─ css/
│  └─ upload.css
└─ js/
   └─ upload.js
```

## Flask에서 사용

```python
@app.route("/")
def upload_form():
    return render_template("upload.html")
```

기존 `/upload` 라우트는 그대로 사용하면 됩니다.
