/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aquincho <aquincho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 11:17:43 by aquincho          #+#    #+#             */
/*   Updated: 2023/03/03 10:40:31 by aquincho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub.h"

static int	ft_map(t_data *data, int fd, char **line)
{
	char	**tmp;

	while (*line && (!ft_strncmp(*line, "1", 1) || !ft_strncmp(*line, " ", 1)))
	{
		if (ft_check_inset(*line, MAP_CHAR))
		{
			free (*line);
			return (EXIT_FAILURE);
		}
		data->height++;
		if ((int)ft_strlen(*line) > data->width)
			data->width = ft_strlen(*line);
		tmp = ft_tabdup_addline(data->map, *line, data->height);
		if (!tmp)
			return (EXIT_FAILURE);
		ft_free_tab(data->map);
		data->map = ft_tabdup_addline(tmp, NULL, data->height);
		if (!data->map)
			return (EXIT_FAILURE);
		ft_free_tab(tmp);
		free(*line);
		*line = get_next_line(fd);
	}
	if (ft_check_map(data->map, data))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int	ft_texture(char **texture, char *line)
{
	char **tmp;

	tmp = ft_split(line, ' ');
	if (tmp[1])
		*texture = ft_strdup(tmp[1]);
	texture[0][ft_strlen(*texture) - 1] = '\0';
	ft_free_tab(tmp);
	if (ft_check_file(*texture))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int	ft_color(t_color *color, char *line)
{
	char	**tmp;
	int		i;

	while (!ft_isdigit(*line)) 
		line++;
	if (!*line)
		return (EXIT_FAILURE);
	tmp = ft_split(line, ',');
	i = 0;
	while(tmp[i])
		i++;
	if (i != 3)
	{
		ft_free_tab(tmp);
		return (EXIT_FAILURE);
	}
	color->r = ft_atoi(tmp[0]);
	color->g = ft_atoi(tmp[1]);
	color->b = ft_atoi(tmp[2]);
	ft_free_tab(tmp);
	return (EXIT_SUCCESS);
}

static int	ft_parser(int fd, t_data *data)
{
	char	*tmp;

	tmp = get_next_line(fd);
	while (tmp)
	{
		if (!ft_strncmp(tmp, "NO", 2) && ft_texture(&data->texture[north], tmp))
			return (ft_error(file_err, " Cannot read north image file"));
		else if (!ft_strncmp(tmp, "SO", 2) && ft_texture(&data->texture[south], tmp))
			return (ft_error(file_err, " Cannot read south image file"));
		else if (!ft_strncmp(tmp, "WE", 2) && ft_texture(&data->texture[west], tmp))
			return (ft_error(file_err, " Cannot read west image file"));
		else if (!ft_strncmp(tmp, "EA", 2) && ft_texture(&data->texture[east], tmp))
			return (ft_error(file_err, " Cannot read east image file"));
		else if (!ft_strncmp(tmp, "C", 1) && ft_color(&data->ceil, tmp))
			return (ft_error(file_err, " Cannot read color"));
		else if (!ft_strncmp(tmp, "F", 1) && ft_color(&data->floor, tmp))
			return (ft_error(file_err, " Cannot read color"));
		else if ((!ft_strncmp(tmp, "1", 1) || !ft_strncmp(tmp, " ", 1))
			&& ft_map(data, fd, &tmp))
			return (ft_error(file_err, " Cannot parse map"));
		if (tmp)
			free(tmp);
		tmp = get_next_line(fd);
	}
	if (tmp)
		free(tmp);
	return (0);
}

int	ft_read_file(t_game *game, char *arg)
{
	int		fd;

	if (ft_strlen(arg) < 4
		|| ft_strncmp((arg + ft_strlen(arg)- 4), ".cub", 4))
	{
		return (ft_error(file_err, " Wrong map format."));
	}
	game->data.name = ft_substr(arg, 0, ft_strlen(arg) - 4);
	fd = open(arg, O_RDONLY);
	if (fd < 1)
		return (ft_error(file_err, arg));
	if (ft_parser(fd, &game->data))
		return (EXIT_FAILURE);
	close(fd);
	return (0);
}