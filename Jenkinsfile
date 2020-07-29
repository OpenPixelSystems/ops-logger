pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh "meson build"
                sh "ninja -C build"
            }
        }
        stage('Cross-Build') {
            steps {
                sh "meson --cross-file .stm32f76x-cross.build cortex-build"
                sh "ninja -C cortex-build"
            }
        }
        stage('Test') {
            steps {
                sh "ninja -C build test"
            }
        }
    }
}
