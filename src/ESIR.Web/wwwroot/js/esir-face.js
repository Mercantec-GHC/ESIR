// ESIR face: the eyes look toward the mouse pointer, and clicking the face makes it hop and say something.
(function () {
    var root = document.documentElement;
    var calm = window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches;

    if (!calm) {
        window.addEventListener('pointermove', function (e) {
            root.style.setProperty('--look-x', ((e.clientX / window.innerWidth - 0.5) * 2).toFixed(2));
            root.style.setProperty('--look-y', ((e.clientY / window.innerHeight - 0.5) * 2).toFixed(2));
        }, { passive: true });
    }

    var phrases = ['Hi!', 'That tickles!', "I'm here.", 'Want to breathe?', 'Hello, friend!', 'You are doing fine.'];
    var next = 0;

    document.addEventListener('click', function (e) {
        var esir = e.target.closest('.esir');
        if (!esir) return;
        var face = esir.querySelector('.face');
        var bubble = esir.querySelector('.bubble');
        if (face && !calm) {
            face.classList.remove('hop');
            void face.offsetWidth;
            face.classList.add('hop');
        }
        if (bubble) {
            bubble.textContent = phrases[next++ % phrases.length];
            bubble.classList.add('show');
            clearTimeout(bubble._t);
            bubble._t = setTimeout(function () { bubble.classList.remove('show'); }, 2000);
        }
    });
})();