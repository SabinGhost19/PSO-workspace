void DIE(bool state, const char *err_msg)
{
    if (state)
    {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}
// void DIE(int state_code, const char *err_msg)
// {
//     if (state_code < 0)
//     {
//         perror(err_msg);
//         exit(EXIT_FAILURE);
//     }
// }