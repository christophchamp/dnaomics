       SUBROUTINE MATCH(N,P,Q,N2,Q2,SSTOTAL,SSTOTAL2)
       double precision P(10000,3),Q(10000,3),SSTOTAL,Q2(10000,3) 
       double precision SSTOTAL2
       double precision PB(3), QB(3), A(3,3), R(3,3), F(3,3), B(3)

C      ZERO EVERYTHING
       DO 20 J = 1,3
          PB(J) = 0.0
          QB(J) = 0.0
          DO 10 I = 1,3
             A(I,J) = 0.0
 10       CONTINUE
 20    CONTINUE

C      COMPUTE THE CENTROIDS AND TRANSLATE BOTH SETS OF POINTS TO HAVE 
C      CENTROIDS AT (0,0,0).

       DO 40 I = 1,N
          DO 30 J = 1,3
             PB(J) = PB(J) + P(I,J)
             QB(J) = QB(J) + Q(I,J)
 30       CONTINUE
 40    CONTINUE

       DO 50 J = 1,3
          PB(J) = PB(J)/FLOAT(N)
          QB(J) = QB(J)/FLOAT(N)
 50    CONTINUE

       DO 70 I = 1,N
          DO 60 J = 1,3
             P(I,J) = P(I,J) - PB(J)
             Q(I,J) = Q(I,J) - QB(J)
 60       CONTINUE
 70    CONTINUE

       ss = 0.0
       do 74 i = 1, N
          do 73 j = 1, 3
             ss = ss + (p(i,j)-q(i,j))**2
 73       continue
 74    continue
       ss = sqrt(ss)
c       print*, ' ss =  ',ss,N


       DO 75 I = 1, N2
          DO 65 J = 1,3
             Q2(I,J) = Q2(I,J) - QB(J)
 65       CONTINUE
 75    CONTINUE


C      COMPUTE THE MATRIX A = SUMM PI*QI-TRANSPOSE AND THE MAX SUM 
C      OF SQUARES  PP + QQ 

       PP = 0.0
       QQ = 0.0
       DO 100 K = 1,N
          DO 90 I = 1,3
             PP = PP + P(K,I)*P(K,I)
             QQ = QQ + Q(K,I)*Q(K,I)
             DO 80 J = 1,3
                A(I,J) = A(I,J) + P(K,I)*Q(K,J)
 80          CONTINUE
 90       CONTINUE
 100   CONTINUE
       PI = 4.0*ATAN(1.0)
       SUMSQS = 0.0
       ALPHSAVE = 0.0
       BETASAVE = 0.0
       GAMMSAVE = 0.0

C      SEARCH FOR BETA IN [-PI/2,PI/2], GAMMA IN [0,2*PI), BEST ALPHA
C      FOR THE LARGEST VALUE OF (P-TRANSPOSE)*R*Q.  THE PROCEDURE COULD BE 
C      ACCELERATED BY COMPUTING AND STORING SINE AND COSINE VALUES.

       DO 140 I = 0, 47
          GAMMA = I*PI/24.0
          SC = SIN(GAMMA)
          CC = COS(GAMMA)
          DO 130 J = -12, 12
             BETA = J*PI/24.0 
             SB = SIN(BETA)
             CB = COS(BETA)

             ALPHANUM = A(2,1)*CB - A(2,2)*SB*SC - A(1,2)*CC 
     1         - A(2,3)*SB*CC + A(1,3)*SC
             ALPHADEN = A(1,1)*CB - A(1,2)*SB*SC + A(2,2)*CC   
     1         - A(1,3)*SB*CC - A(2,3)*SC

C      ALPHA IS CHOSEN SO THAT THE FIRST DERIVATIVE OF P-TRANSPOSE*R*Q
C      IS ZERO AND THE SECOND DERIVATIVE IS NEGATIVE.  THUS P-TRANSPOSE*R*Q
C      IS MAXIMUM FOR THE VALUES OF GAMMA AND BETA THAT ARE BEING TESTED.
             IF (ALPHADEN.EQ.0.0) THEN
                IF (ALPHANUM.GE.0.0) THEN
                   ALPHA = PI/2.0
                ELSE
                   ALPHA = -PI/2.0
                END IF
             ELSE
                ALPHA = ATAN(ALPHANUM/ALPHADEN)
             END IF
             IF (ALPHADEN.LT.0.0) ALPHA = ALPHA+PI

C      NOW TEST THE SUM OF SQUARES (PP + QQ - 2*P-trans RQ = PP + QQ - 2*SS)
C      FOR THIS ROTATION OF Q

             SA = SIN(ALPHA)
             CA = COS(ALPHA)

             R(1,1) =   CA*CB
             R(1,2) =  -CA*SB*SC - SA*CC
             R(1,3) =  -CA*SB*CC + SA*SC
             R(2,1) =   SA*CB
             R(2,2) =  -SA*SB*SC + CA*CC
             R(2,3) =  -SA*SB*CC - CA*SC
             R(3,1) =   SB
             R(3,2) =   CB*SC
             R(3,3) =   CB*CC

             SS = 0.0
             DO 120 II = 1,3
                DO 110 JJ = 1,3
                   SS = SS + A(II,JJ)*R(II,JJ)
 110            CONTINUE
 120         CONTINUE
 121      format(2f8.3, 2x, 3f8.3,2i4)
c          print*,  ss, SUMSQS,i,j
             IF (SS .GT. (SUMSQS)) THEN
c          print 121,  ss, SUMSQS,alpha, beta, gamma,i,j
                SUMSQS = SS
                ALPHSAVE = ALPHA
                BETASAVE = BETA
                GAMMSAVE = GAMMA
             END IF
 130      CONTINUE
 140   CONTINUE

       ALPHA = ALPHSAVE
       BETA  = BETASAVE
       GAMMA = GAMMSAVE
       SA = SIN(ALPHA)
       CA = COS(ALPHA)
       SB = SIN(BETA)
       CB = COS(BETA)
       SC = SIN(GAMMA)
       CC = COS(GAMMA)
c	print*,GAMMA,BETA,ALPHA
C      NOW DO 10 NEWTON'S METHOD ITERATIONS TO IMPROVE THE SOLUTION
 
       DO 170 I = 1,10

          ALPHANUM = A(2,1)*CB - A(2,2)*SB*SC - A(1,2)*CC 
     1         - A(2,3)*SB*CC + A(1,3)*SC
 
          ALPHADEN = A(1,1)*CB - A(1,2)*SB*SC + A(2,2)*CC   
     1         - A(1,3)*SB*CC - A(2,3)*SC

          BETANUM  = A(3,1)    - A(1,2)*CA*SC - A(2,2)*SA*SC 
     1         - A(1,3)*CA*CC - A(2,3)*SA*CC

          BETADEN  = A(1,1)*CA + A(2,1)*SA + A(3,2)*SC 
     1         + A(3,3)*CC

          GAMMANUM = -A(1,2)*CA*SB - A(2,2)*SA*SB + A(3,2)*CB 
     1         + A(1,3)*SA - A(2,3)*CA 

          GAMMADEN = -A(1,2)*SA + A(2,2)*CA - A(1,3)*CA*SB 
     1         - A(2,3)*SA*SB + A(3,3)*CB

          B(1)   = -SA*ALPHADEN  +  CA*ALPHANUM
          B(2)   = -SB*BETADEN   +  CB*BETANUM
          B(3)   = -SC*GAMMADEN  +  CC*GAMMANUM

          F(1,1) = -CA*ALPHADEN  -  SA*ALPHANUM

          F(1,2) = - SA*(-A(1,1)*SB - A(1,2)*CB*SC - A(1,3)*CB*CC)
     1       + CA*(-A(2,1)*SB - A(2,2)*CB*SC - A(2,3)*CB*CC)

          F(1,3) = - SA*(-A(1,2)*SB*CC - A(2,2)*SC + A(1,3)*SB*SC 
     1                                             - A(2,3)*CC)
     1    + CA*(-A(2,2)*SB*CC + A(1,2)*SC + A(1,3)*CC + A(2,3)*SB*SC)

          F(2,1) =  F(1,2)
          F(2,2) = -CB*BETADEN  -  SB*BETANUM

          F(2,3) = - SB*(A(3,2)*CC - A(3,3)*SC) + CB*(-A(1,2)*CA*CC 
     1    - A(2,2)*SA*CC + A(1,3)*CA*SC + A(2,3)*SA*SC)

          F(3,1) =  F(1,3)
          F(3,2) =  F(2,3)
          F(3,3) = -CC*GAMMADEN  -  SC*GAMMANUM

          determ = f(1,1)*f(2,2)*f(3,3) - f(1,1)*f(2,3)*f(3,2)
          determ = determ - f(1,2)*f(2,1)*f(3,3) + f(1,2)*f(2,3)*f(3,1)
          determ = determ + f(1,3)*f(2,1)*f(3,2) - f(1,3)*f(2,2)*f(3,1)
c	print*,determ
C         THE SUBROUTINE SOLVE IS A VERY SIMPLE GAUSSIAN ELIMINATION
C         ROUTINE, WITH NO PIVOTING.  BECAUSE F SHOULD BE POSITIVE DEFINITE
C         THERE SHOULD BE NO TROUBLE.  A MORE CAREFUL ROUTINE COULD BE USED.

c	print*,b,f
          CALL SOLVE(3,F,B)
c	print*,b,f

c         do not move too far from the starting point

c          test = sqrt(b(1)*b(1) + b(2)*b(2) + b(3)*b(3))
c          if (test.ge.0.1) then
c             scale = 0.1/test
c             b(1) = b(1)*scale
c             b(2) = b(2)*scale
c             b(3) = b(3)*scale
c          endif
c          print*, f, b
c          print*, ALPHsave, BETAsave,GAMMsave
          ALPHA = ALPHsave - B(1)
          BETA  = BETAsave  - B(2)
          GAMMA = GAMMsave - B(3)
          SA = SIN(ALPHA)
          CA = COS(ALPHA)
          SB = SIN(BETA)
          CB = COS(BETA)
          SC = SIN(GAMMA)
          CC = COS(GAMMA)

          R(1,1) =   CA*CB
          R(1,2) =  -CA*SB*SC - SA*CC
          R(1,3) =  -CA*SB*CC + SA*SC
          R(2,1) =   SA*CB
          R(2,2) =  -SA*SB*SC + CA*CC
          R(2,3) =  -SA*SB*CC - CA*SC
          R(3,1) =   SB
          R(3,2) =   CB*SC
          R(3,3) =   CB*CC

          SS = 0.0
          DO 160 II = 1,3
             DO 150 JJ = 1,3
                SS = SS + A(II,JJ)*R(II,JJ)
 150         CONTINUE
 160      CONTINUE
c          print 123, ss, alpha, beta, gamma, i
 123      format(f8.3, 2x, 3f8.3,2x,i5)
             IF (SS .GT. SUMSQS) THEN
                SUMSQS = SS
                ALPHSAVE = ALPHA
                BETASAVE = BETA
                GAMMSAVE = GAMMA
             END IF

 170   CONTINUE

       ALPHA = ALPHSAVE
       BETA  = BETASAVE
       GAMMA = GAMMSAVE
	write(*,*) ALPHA*180./3.1416,BETA*180./3.1416,GAMMA*180./3.1416

c      do 172 iii=1,3
c           write(*,175) (r(iii,jjj),jjj=1,3)
c172   continue
 175   format(3f8.3)

C      ROTATE Q AND Q2 THROUGH THE ANGLES ALPH, BETA, GAMMA

       DO 180 I = 1,N
          TEMP1  = R(1,1)*Q(I,1) + R(1,2)*Q(I,2) + R(1,3)*Q(I,3)
          TEMP2  = R(2,1)*Q(I,1) + R(2,2)*Q(I,2) + R(2,3)*Q(I,3)
          TEMP3  = R(3,1)*Q(I,1) + R(3,2)*Q(I,2) + R(3,3)*Q(I,3)
          Q(I,1) = TEMP1
          Q(I,2) = TEMP2
          Q(I,3) = TEMP3
 180   CONTINUE

       DO 185 I = 1,N2
          TEMP1  = R(1,1)*Q2(I,1) + R(1,2)*Q2(I,2) + R(1,3)*Q2(I,3)
          TEMP2  = R(2,1)*Q2(I,1) + R(2,2)*Q2(I,2) + R(2,3)*Q2(I,3)
          TEMP3  = R(3,1)*Q2(I,1) + R(3,2)*Q2(I,2) + R(3,3)*Q2(I,3)
          Q2(I,1) = TEMP1
          Q2(I,2) = TEMP2
          Q2(I,3) = TEMP3
 185   CONTINUE

C      TRANSLATE BACK TO THE ORIGINAL P CENTROID

       SSTOTAL = 0.0
       DO 200 I = 1,N
          SS = 0.0
          DO 190 J = 1,3
             P(I,J) = P(I,J) + PB(J)
             Q(I,J) = Q(I,J) + PB(J)
             SS = SS + (P(I,J)-Q(I,J))**2
 190      CONTINUE
          SSTOTAL = SSTOTAL + SS
 200   CONTINUE
       SSTOTAL = SQRT(SSTOTAL)

       DO 220 I = 1, N2
          SS = 0.0
          DO 210 J = 1, 3
             Q2(I,J) = Q2(I,J) + PB(J)
 210      CONTINUE
 220   CONTINUE

       RETURN
       END

C********************************************************************************

       SUBROUTINE SOLVE(NS,F,B)
       double precision F(3,3), B(3)

       DO 30 K = 1, NS-1
          DO 20 I = K + 1, NS
             F(I,K) = -F(I,K)/F(K,K)
             DO 10 J = K+1, NS
                F(I,J) = F(I,J) + F(I,K)*F(K,J)
 10          CONTINUE
             B(I) = B(I) + F(I,K)*B(K)
 20    CONTINUE
 30    CONTINUE

       B(NS) = B(NS)/F(NS,NS)
       DO 50 I = NS-1, 1, -1
          DO 40 J = I + 1, NS
             B(I) = B(I) - F(I,J)*B(J)
 40       CONTINUE
          B(I) = B(I)/F(I,I)
 50   CONTINUE

      RETURN
      END

