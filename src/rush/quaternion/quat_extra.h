//
// Created by gaelr on 07/09/2023.
//

#ifndef RUSH_QUAT_EXTRA_H
#define RUSH_QUAT_EXTRA_H

template<typename Type>
std::ostream&
operator<<(std::ostream& os, const rush::Quat<Type>& q) {
    os << "(" << q.s << ", {" << q.x << ", " << q.y << ", " << q.z << "})";
    return os;
}


#endif //RUSH_QUAT_EXTRA_H
