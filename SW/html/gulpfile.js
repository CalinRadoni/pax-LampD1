const gulp = require('gulp');
const inline = require('gulp-inline');
const terser = require('gulp-terser');
const cleancss = require('gulp-clean-css');
const favicon = require('gulp-base64-favicon');
const htmlmin = require('gulp-htmlmin');
const gzip = require('gulp-gzip');
const del = require('del');

const srcDir = 'src/';
const dstDir = 'static/';

function clean() {
    return del([dstDir + '*']);
}

function build_html() {
    let stream = gulp.src(srcDir + '*.html')
        .pipe(favicon())
        .pipe(inline({
            base: srcDir,
            js: terser,
            css: cleancss,
            disabledTypes: ['svg', 'img']
        }))
        .pipe(htmlmin({
            collapseWhitespace: true,
            removeComments: true,
            minifyCSS: true,
            minifyJS: true
       }))
       .pipe(gulp.dest(dstDir))
       .pipe(gzip())
       .pipe(gulp.dest(dstDir));

    return stream;
}

exports.default = build_html;
exports.clean = clean;
