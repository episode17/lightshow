<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>LightShow</title>
    <link rel="shortcut icon" href="favicon.png">
    <link rel="stylesheet" href="css/style.css">
    <script src="libs/jquery.min.js"></script>
    <script src="libs/mustache.min.js"></script>
</head>
<body>
    <div class="ls-wrapper">
        <div class="ls-header">
            <strong>Ligth Show Simulator</strong>
        </div>
        <div class="ls-content">
            <canvas class="js-ls-canvas" width="200" height="660"></canvas>
        </div>
    </div>
    <script src="js/app.js"></script>
    <script>
        $(function() {
            var app = new App(
                $('.js-ls-canvas')[0], 
                120
            );
            
            app.start();
        });
    </script>
</body>
</html>