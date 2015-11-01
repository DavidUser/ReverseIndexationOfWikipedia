output

hp = @(X) plot(histc(X, unique (X)))

figure;
hp(RS);
title("Robert Sedgwiks colisions");
figure;
hist(RS);
title("Robert Sedgwiks values");

figure;
hp(JS);
title("Justin Sobel colisions");
figure;
hist(JS);
title("Justin Sobel values");

figure;
hp(BKDR);
title("Brian Kernighan and Dennis Ritchie's colisions");
figure;
hist(BKDR);
title("Brian Kernighan and Dennis Ritchie's values");

figure;
hp(DJB);
title("Professor Daniel J. Bernstein colisions");
figure;
hist(DJB);
title("Professor Daniel J. Bernstein values");

figure;
hp(ELF);
title("ELF colisions");
figure;
hist(ELF);
title("ELF values");

pause();
