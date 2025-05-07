from PIL import Image
from io import BytesIO
from fastapi import FastAPI, File, UploadFile
from ray import serve
from transformers import pipeline
import ray
import wikipedia

def fetch_wikipedia_page(search_term,chars=500):   
    ## Completar
    ## Si página no encontrada o no hay entradas asociadas
    ## a ese termino de búsqueda devolver "No pages found".
    ## En otro caso, retornar un string con los chars primeros
    ## caracteres de la página que aparece en primer lugar
    ## al hacer la búsqueda en Wikipedia
    results = wikipedia.search(search_term)
    if results is None or len(results) == 0:
        return "No pages found"
    
    try:
        page = wikipedia.page(results[0])
    except Exception:
        return "No pages found"

    return page.content[:chars]

app = FastAPI()

@serve.deployment()
class WikipediaSearch:
    async def __call__(self, input_text):
        contents = fetch_wikipedia_page(input_text)
        return contents

@serve.deployment()
@serve.ingress(app)
class ImageClassifier:
    def __init__(self, wikipedia_search):
        ## Inicializar atributos 
        self._classifier = pipeline("image-classification")
        self._wiki= wikipedia_search
        
    def classify_image(self,image):
                ## Completar
        ## Ejecutar modelo self._classifier() sobre imagen. 
        ## Si no hay resultados o puntuación (score) es < 0.80
        ## devolver None
        ## En caso contrario quedarse con el mejor resultado
        ## y con primer término (en caso de que hayq varios sinónimos
        ## separados por comas)

        preds = self._classifier(image)
        if (len(preds) == 0) or (preds[0]["score"] < 0.8):
            return None

        return preds[0]["label"].split(",")[0] # por si hay sinonimos
        

        
    @app.post("/whatis")
    async def classify(self, file: UploadFile = File(...)):
        ## Extrae fichero de imagen de petición HTTP
        contents = await file.read()
        ## Convierte contenido a imagen PIL
        image = Image.open(BytesIO(contents)).convert("RGB")
        ## Invocar modelo para obtener término de búsqueda
        search_item = self.classify_image(image)
        
        if search_item is None:
            return { "prediction": "unknown"}

        ## Completar conexión con modelo de WikipediaSearch
        page_contents = await self._wiki.remote(search_item)
        
        return {"prediction": search_item, "wikipedia info": page_contents }
        


wiki=WikipediaSearch.bind()
info=ImageClassifier.bind(wiki)