const char* index_html = R"INDEX_HTML(
<html>
    <head>
        <title>Start page</title>
        <style>
            
            {{ common_css }}

            body {
                font-family: "Times New Roman", Times, serif;
                text-align: center;
            }
            h1 {
                font-size: 6em;
            }
            p {
                font-size: 1.65em;
            }

        </style>
        <script>

            {{ common_js }}


            // USER INTERFACE (INPUTS)

            function onStartBtnClick() {
                hide('panel');
                show('preloader');
                apiStart(reload);
            }

            // USER INTERFACE (VIEWS)

            // ...

            // COMMUNICATION

            function apiStart(success, faliure) {
                ajax("GET", "/start", {}, success, faliure);
            }

        </script>
    </head>
    <body>

        {{ common_html }}

        <div id="panel">
            <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFMAAABFCAYAAAA2PdqGAAAXRElEQVR4Xu2cCZgV1ZXHf/fe2l6v7KsIKotGFlnEIEQjKKJBcRREGSHROBnDjI5JlEhMYjJG3BLHRDOZiSgGAcEFUDAiUYn6mbiBcUE2kSVsKrL28t6rqnvnu/c1BEyE7qYbmczU9/HRSr3qer8699xz/uecEsYYw/8fDUJA/D/MBuHoLvK5w0xJibWges2vye58nkwC+eIq/FhR3uvnCNMBIVXDfeNGvNLnANOAERhR8C4i1Xyy5j/xP3wBrWJ8IanKgCg+lebHXofQBpRsRAQNd+nDD9OxzGHyITJ5hY1L7qFE5zB+ipQSIQRGGUp7/wohWzXcNz0MVzrsMA2GxAiWLRpDh1wWE2ZB+chA7oXpG0Nw8uPI/x0GufcxHTaYCVVIFCL5hHXPfY1i6TsrFFKirEv0faxrtP8v9ULKe09HiMNgTg34Kw4bTJMC2bWsf/FqSuOYNKMKMD2NEc3xgoq9lqk6X0ZadjYZfDzPa8Cv27iXalSYhhiR+iATSLey5qlLiCgCJZAyQpfkSURzOpx8M7tfm0Aa5TFhnqY95lEpY+5/YTFXDzwF4f3vMNFGhUkKqUoweGyZfzpp2hRf5JwFBlIQdvoKQber2Pr2RZTuikgyWapkCe36TiUWhhuffo3TurRmeOdOjWtSDXT1RoWZUoXSEe/PvYLIbEYpRVamNJMewfBHUMsfpWLLXBvtkkaGsjgg6P8bRFAMBia/+AfWVEZMOKcP5SIB48ERbKSNCtMkQO5l1j/5fVQQOJh5XcHRQ2cjQo8dL48jER6SFBEIhMhQ1ncK0o+crSza8CG/e2sDxYHhe0P6kcjEetEGsqOGv0yjwtRmG+unj0IHEl8KfN+nzbDp5IIyti+6gFjFlCeCxPMJlMeO4jLanTQFWWN9f9y6k/mvrSYfQu+2JYzp1sX52yP1aByY1iSrt7PyiVEEClQa0Pa8m1CZAaA0VVufp3LJ3YXd2/5RAl/6lJz6M7TsuJfXT597id25MrJBSuwLmiWVfO/0gW5DOhKRNg5MDYunnUsbkyMbSEKg3SW/Q4gYTMzO+ZcQR+neONPCdGHQ6VPxU7vbF2xvwpPPEpmWxEUpeIJiLfnXIb0oNzYePfLss1FgmlTzwczT0FIRaUlleTe6nfsr9+0XP3Elx3gb0JG3H8wkgpb95yDsRoMmnwbcuPANZ71SCedvpRBc3Oc4Tigr2cP7iCLaKDCzVLH5wa+4XTpfGtDl/IXEHnjZ9/jw2WuI0hBrgHsyIGuZu8IutD3lZkITgkzZajxuX/D6fjCtS2iTVnPV4FP+vmHaBEdSSZIv5p1ZX6bYaNqddT2ZZsPxwoTtKx4mXjGDBBuwS1RYgGl/TkNDi1NnIkSxszSrJ333iRcxQfRXlpkGklsG9DgiQ6SGs0ynBlUhcoLVs85ha3Q0J4+aatU2lIEts89CCkXi7Q9TCUmqQlqc9qh9GoUjTfm3Z14jEOqvYCa+YNKpPf8PwDSw8pUbke//kc5j5iE8CTrCCM2m2cOJA0lg9H6WaXd3z5RTOngayi1eTUzKDU8tQcoC+H19ZiwFt37p7xxmgoWSsvLBYQQetB87D88UIUjQpoJNcy8ClUGKgm7p+crplkIZqvwSjho0E6ktSsMd857nIz9CCX9/mFaT04abzuj19+0zyUPV+jn8+ZVf0HXMfISwFpmHrM/qJ4cQeEX4loXEAcqEilykaDVgColXQmokUSz4tznzIWqK5wVOz9zPMqV0/vRHp/f8+4aZEvPW/UMp7dCPzkNvt9sMqYYNb9yCv+oFdFQIb7QyhU0ngqO+NJ1EleApD5KYN3ZUMPmldwmlcNKbA7nvMpeCREpuHtjjiKxkNNgGpElYPeMf6HzRQ5iwidtLYq1ZNussWmoLz4rABZh2F2/e/0pUiwuRsgJJCVbvvHru0yDKUMp8Jsy89Ll10AnuGkfa0WAw0bD23btoeeJVFKnQLUdTsYwNc67GhAZPhQ5m4huCVND6/DlgfIyxdR/Dw68v56WN24jShNRXnwkzFgG3DrQ5+pEneDQczH3NRNswaTPrp1+MMEWYIEZ5voO5MwzoNsxmOgXLMmgqkVw/6wU8JxpLl07+7WUu8YzggpO60KM8POKsswFgashLqoIKirJFGCkQvmbVou+gNr9dqPHY4DyTJx+04JizZ4L0rW5MrLMEIuJf7puOKmuP9Ao+8rNg2mtpIQg9w/f790R4MUJbJf/IWPCHDtNALCrYtvhBgh1PkTQdQste1/HurGEU63yNMiTIRxHHnTcHI2qAmYRUK26b9wxr8xGBpnYwlcATig5NfK7s2hnta1eoOxKOQ4ZpfaNIUza+NIJiYYh6X0dUOoi1/zUUU1pYtkIKjrpwHto3eCKzN2V86OW3+f2aj1FSE9hyr6qFZdaIHr7wGNnzGLplPCeAHAlHvWHa3dsYq/z8mXVz/4lMU0u1jNanTUEYyfJp5xEiiESO4jMmUNZ2OKlMXGqZN5Jv3jefJMwQqAQhPCcc1wWmp2syz8jw4z5fqLcml6JRqeT+NxZzWZ/eBL7NVOvnNw4BZh6Zi1n99EgytkAWSqrLOtF6wN141R+yefpYclFKpd+aHqNmYtz9CVIDr6zaxIMvvoWxdXIrrdWo8HWF6TYxTzGgZSlDjmlbL8HYCn5VGu5cspKJ/bqSQR9+mImGlb+9kqa5j9BBTBRB09NuQfh9WPPoxZh4C74I6XDxDBK/DEWINLAul+emKc8gAh/lXIAFWihp1AWmezS2YUEJSpMc1w2oZ4qp4ZE33+MDWUKvpoJhHdsghF8vr1FvyyTewqY5lxIHCk/5RKqY5sMeJk+O1VPPxxcawua0uPhxmti9Wxp2xYJr/vtJYi8k8GWDwFSJJOPn+M4pPeu1OLPETFryAVFiM9+EG086kbCeLrjOMK0QYTSsm3UWUhp8T5IvUsg2A+jYayLbNs5nx3P3sj0spu+l8/Y+4avueoDdqjXGT1y8aePIulrmnsYu20/T0WjOH9SdFvWwIfsdpDbsSjX3LF6G9oL9rvKDvseCDbnqGCTUGabtM7Z/1s4c4nqDPE+RZgQdh9yLDrqwdM4w/MqUbmN+g1DtCju3MVxwz29poXdTHWQOGab0NN8/uSdKJmhlS8V1M6W8TlHS8MCr77FJRsia9sY9RK/v1w2fBFXHsnKdYUKKbXtZM+Mre2HqopSjz5pDqspYO3U4OviIzpe+jKDge7TWjLn3MbTM4El9yDDtpnVG2+b0P7Y5Ge3VI2jPMX3xB6wSIUrnXI1g3yNMdnNd/3517sKrM8y8SVn68HDKyRdg+grh+bQ490lE1etsmT2BvDR0vuwlEDGbE8W/3PUYYVjQJm1TRn2XucuAlHAKvJDVXP/FPriotZZxpo2mpNbctGQZ2naVWD3QigqfOlIECsO3+3al2Pp7UTsdoM4w4xQ2Th2MDkUNTIn0Fa3OW8A7C75Nsy2vkvWa0W3kPFfbGfPj+6C4mTunoWCW6YTxp/WmNJuSRHVYjEaTE5LbX32fAEMqrL66P0z7X75JEEbh53bwnS/1r3WoVGeYdhdfPXMkQoUOpoVUpAzlIxawdMYIIvkJLbqPp8UJY3l7+3Z+MOVpAu3hNYBlOh1UCUb2Op6TSn2E0+VtQ8JBfGbNPIn967G332N5bC1y/88Io9FCkthYjRhf22UH43t0obktv9TiqDPMHRueZPfCu4mjgmVKX+J5kqbnPcWfpw4jFDEdL3uGWGgu+veHMEUlFGkPGVrl/NCWuf28EzpUytj+PejoW4y1+KIGciYllYpfL1nB7r9e2cRoupSVclQUsOjjHfjGxS0U5Sq5bmC/WqCsx7TFOw8Mo9SrQHsFy7Q+M4mK6DRsGu//5hxi2ZwTxs3mnyb9mo9lmbsJ6yODoGFgpkJQLDRndj2KPq2b4Tmfd7AYJqUqUfz0zfcQ4jPO1ZKAHFd1P46s1jyzbC3vKw+hBT/q24VYQKDFATe7Olvm2gfPIlVZhAocTBtnVkUldB58M6seG0/pF0bRsu/VjL5tJqkNSBsQphVMrGV+77TuFNmAyFqP7T4+2DJPDYu2beXFtZVIYXfvvz5yXowfR1zeqyPluV2INMNDy95jlyzjW6d0xSPGMy5x/8yjzjDfeWgwxbbGYGs0NTArSrvRufsINj5/M+3On4koacbwO2cT1iynhrJMp40awU1f7mIllEK3Qm1CTJ1yz6vL2GY3wc9wC6nMQ1JMjyDL0OM6FIDlJfesXc343t0pF8ZtSgfyKnWG+da0wZTq/WEm7QYSBu2pfHcax45dgPXcI257bG8w3FAwpZDkPMntA7offGXvYz+pznPvmx+wM00Q8m/n3VbRIg1omt3NFSd1cZ+2uf82E/HaylWM7NPNabEHenZ1hrl6ypdRIkVbH1izm7focwubFt9PVLGSdl9dSDUeX5001TWyfnqZezIlF/pkXENrSqCkK1cIXyC9gFiDChS+jSdtvCc9l9m5kQyp8ISgSb6K64Z8sVabgksajOanf3yfOLQ1/IP5V/hW13YkMsXTyrmqu1Zt5of9uqCczNWAy/y9+88i42cxtq5dA/Posx/grScmUFK1kY5f/z3vbt7KT6YsIKkZ09vXMhMlKRMpt4+/gFIX07nSW6GgLqA6hlwI0555nU0mR7WnKNYZjG1aEAKtFN8e2M3l5K7EUZsjTbj7leXsjhQ1WuABP3Vtl7akttNEK4SS/GzZBn7Qt7ML5RsU5ttThlPkb3fTEnthDn+Y12ddSYu0mvZfW8Bt0+fz9tpPSGqW1L4wWxXnufXro4hMgq6xXIvEuDTV5iM2CI8LztBG1UJZ10W8xz9qKLY/H9zA/gLMxDyweAXrhe/E6QMd9gFdc1zrvTBtr9S9yzcxsV9nnAB7gOdX52W++PGRlGU3o0QBpg2N2l84g6WTv0EaZOkx7klG/3gqqR9hI4k9y9wW0E7t2pLrR5xx2KuKeWDjrhyTV61z/j6bCkxgG8pSfO2R2LAngZwooon+iMu7d3X3nfU0RXnFvBXruKR/V1eablDLZMufWL5wIoG3JzeXtP+Hmayf+V3y+XUcM24hl06aRirUfjAfnzCClKhOBlWbFVybc1xpQsPkN5eyJY34Ro+jyCR5jNbkVERsJUHrcpIYmWpnJO6ojHlNxJzRopUzShtJNCzMfJYNMwaTDzJ7M6AOFzzE2qfuIP/Ju3T42lOMm/SwbeNymqAUVQRhMdO/c9Fht8hPg7YWOnnxauLKrYzpfSKlqcZmlnaXto25Nk33UqiSCYEn+SQf8OsN67ilx/G1eWZ1nzfXppr1U0ago3Sv0NFu6C/Z8s5ccqt+S8exzzD6lkdc6ia0JFEJzYRhyg1ja3VDjXqSMbyzaxezP9hOUdV2rux7PGGVJvX/kl+6F0PokGe3bOCtfMK3ux1HedhIqlHWwIapZyJ9sxdmcPz1NCnOsuWFn9Fh3GOMuvm3aOW7p1ztSfq0KeLHl5/fqJxqe3GblN26ZCk5AkKd48xj2nFCqFyObwMKOxn3y+VriPI+13zxBDyR4NXosgf7HcIY7YKTG05vT8ZXRL5HmedTXFZEeVRBRXFbBl0xkU69ziVxc/QxxDFrHj1v724uw7a0P/du1swYSfvz7+Le5yr5w9K1SKcXGtq18vj5P4862L0ctn+3kYMNB2yEsGJ3FUtWb7RDxRxdWkzvju0osjOfLue3cyK1P4Q2xmSBO4e2RSUQBYogCigpDykJrHTmoQNDpwGX0Xvk1Ugr6GvJ+lmFsoWw4nDo0+Gc2ayYNhh1zEWkPccz8d45BNo4WSsMckyb8NXa31Ujn+miLKcU2xFC5zQLUp7tH7X1IbvR2JNqGcbuud0ayxRUfriCH40eQtsoISgKKCktJRN4BJEkiHzKiyRVZa0Yeu2viGnLpmlnEBeFlAiN8iStzp3H8sfPIa1qwrGXz2bsLbPQylpAIbie9d3RrlZTS1G8kXEe+PIpCbatVmiPbULw0Ycf07VNSyS2yv4p/+niVo19EjVxpnFTEPZxqHQLP7/yQpqxzaVymaLAwWwWlCODCozw6POtB3hr7g20DHYSiggV5SgfPo9XZ19Os4ot9Bj7Oy6e9ChJTYRsg/ZHvjvKdXrUSpj4XFGC7QmoMpr/WPgCsSrhX8/sSxvbvGs7Uj4FM0uerPFo8heYf/vu88amezZqyPLEL8bQfMcmCqKz4YsTX2DVmzfR8oMlVJV4tB0+C5EvZfWsQfhdRnPNovZQI3c5mBNGF7KP2qg8hwjTtinaX2PzKJNKnnx3Gc+9+idyIiARGVTo4YkUz/PdcvY9DyM9J4LYMCkKbeIoySrDoDBmTeVOrh92NsYNfClu+8Nr3HByf1Iv5qU12+l9XGuaHPRVPFYd2pPc21E97TH/rq+SqfwAv9VxnPrN37Bx9gVQWkXYcjAtT7qBzQt/SPXHLzF517Ws2FnpsFiY//6N8zix1Nai69ctURe+bsbNQEVVnuv/6yESrw3aK8xo6iAldD8HLu7dU9xLgpDQGEKtObt7O3p37kAxWVIdcsdLi/nRoH5opXnhz9v5QofmtNbw+23bWLSughv7HE1g4/kDvSRKuye8p4BiXwORuk7fyq1/YuUvb6D3TY+7WHLNwgtpoiRNzpiB8TKsmnkaXS9+keE/mU0QJi6Pz5DjoevGOKdeq1JDXei5c+3dFgYI7C79z3c+wE6/mAgfGdruOtwcki8CRGD2ti8KT1Guc1w6oDud27ejyE4NmxghU3anEZNe/BPnndCRU1uVMPW1VSzDcGvv7ty/9B1WGg/t+dzZ89h9fWZd7tzugIpPlj3Lq3PuZPDEBRhZwY5F44iEovSYUYh2ozGy2uWyY2+5D1PShsRPCJKE+781hkw9dsqD3aHbGkxMhfG5aNIMSkIbxhVm3EXNm2n2DBx4vp0tKrz2J5Qpt18ylKoaD1SVpPxqwcvstNFlFBJadxCUo8lSSsJ25VOuAxJPUeJlufbkHoXNyin/9XkXnDFkEcybNIoLJzyC8gTGfMi2P15NhiyZfrfx8dLf06rXeGIFl942A6/II1UBXz/3FIZ1bA5y/5aUg8E62L/byQ67ii6/9T52yRKkUg7kwWDGxYo7zh3E3XP/wA6l8YMI5SmU7YC2jtfOLNX0jcaeIGM0O/wiuuaquXxgd0L3AqvCRlA/mFbRN5ogianwAkrsi51srTkVrFtyFU3EZsr6PIaQVgA2fJRLGf/f8/GlPSdl8rWj7WtPGvYwMVtyiivvmIofhIUpuFrAzEh7bgXHdz2Rd9atwVc+qmZsxqXsNX7VDYIZRVUgGdIyw6BOncjYtpp9XsZSb5i24ch6VDuBZptVrbOKRR4v55N++DyqQ0+EaG5DYNJEM+4Xc11jhFaS9hnBT68Y0aAwbVn2sp9MZqdXjGcztVpaplX5PRWBlydRHhn77iW/MIP0aZhCapqn1Vxz2gBnjPYVbPsW8+oP84Aoqknx94vJcqnhqTfeZc7rq/BVEfdcZXvea7KMQwyXrKf62cxFvLx2M74WTkwuwLSvQgvcpIcnTKHrzre1QFvrLyQTqf3b8/DsY3clFI9WCIrDDN06tKDv8R1paoPyOHVwDxSNNArMPW/k3C/bSfNoFbB0/Q5uf/pFOhUbbv7HEU7zrIto/reeYRzHjLvzCXaJPCVW5K3pZ7INXsUq5vjWpVw0dCCtmzXFeep93tXnBCPb2WeznpoyiLVyG1vZz9sfba1e7o1sPvtuGwXmgdev4eN8ysSpzzJh9JfpWmwdU1D/YN5YDTfla3fMInFptiaIcwwd2JPRQ052jV21E9AO3escdpjO0xrDN+95FFnSgluvONMO+xXEhfosd6PZaQRjJ00n8iVptooZP/w6gUhdDFxwfIcOqjZXOOww970p6+ts7+bejuDa3PGnzzGaajsJZEsOojDA+nkdnyvMhvjSTpk8TDn/we73fwAGIS25C/RpfwAAAABJRU5ErkJggg==
">
            <h1>IDUNN</h1>
            <p>Youthfulness in a single drop</p>
            <button onclick="onStartBtnClick()">START</button>
        </div>

    </body>
</html>
)INDEX_HTML";