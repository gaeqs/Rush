# Rush - A _C++20_ mathematics library

_Rush_ is a header only _C++20_ library that implements
mathematics solutions for graphic applications.
Specifically, _Rush_ was created to work together with
modern _OpenGL_ and _Vulkan_ applications, but it's
not limited to these technologies.

_Rush_ takes advantage of several _C++20_ features, such as
concepts, advanced template specifications or new _std_ features.

Currently, _Rush_ includes full support for vectors, matrices and
quaternions.
Future versions may include features such as noise, splines,
advanced data manipulation or diverse algorithms.

## Features

### Vectors

_Rush_ can create vectors of any type and size.
Depending on the vector's type, _Rush_ can perform
operations such as additions, multiplications or iterations.
Specific vector types can perform special operations, such
as cross products for vectors with three components.

```cpp
rush::Vec<3, float> vec1 = { 5.0f, 3.0f, 1.0f };
rush::Vec<3, float> vec2 = vec1(1, 0, 1); // {3.0f, 5.0f, 3.0f}
rush::Vec<2, float> vec3 = vec1(1, 2); // {3.0f, 1.0f}

float a = vec3[1]; // 1.0f

vec3(1, 0) = vec3(0, 1); // Swaps the vector. {1.0f, 3.0f}

// Iterates the vector.
for (float f : vec1) {}

// You can perform basic operations.
rush::Vec<3, float> dot = vec1.dot(vec1.normalized());

// You can also use basic operators if your type support it!
rush::Vec<3, float> result = dot + vec1 * dot;

rush::Vec<2, std::string> stringVec = {"Hello", "World!"};

// Compilation error. String doesn't implement operator "*".
rush::Vec<2, std::string> stringVec2 = stringVec * stringVec;

// {"HelloHello", "World!World!"}
rush::Vec<2, std::string> result = stringVec + stringVec;
```

### Matrices

Just like vectors, _Rush_ can create matrices of any type and size.
Depending on the matrix's type, _Rush_ can perform operations
such as additions, matrix multiplications or iterations.
Matrices are stored in colum-major format.